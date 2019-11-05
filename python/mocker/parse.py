# Copyright 2019 Ole Kliemann, Malte Kliemann
#
# This file is part of DrMock.
#
# DrMock is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# DrMock is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with DrMock.  If not, see <https://www.gnu.org/licenses/>.

from __future__ import annotations
import configparser
from pkg_resources import resource_filename
import clang
from clang import cindex
from clang.cindex import CursorKind
from copy import copy, deepcopy
from dataclasses import dataclass, field
import json
from itertools import count
import subprocess
import sys
import platform
import re
from typing import Generator, List, Tuple, Union
import uuid
from . import utils

# Python clang dictionary:
#
# namespace                     NAMESPACE
# class                         CLASS_DECL, CLASS_TEMPLATE
# using                         USING_DIRECTIVE
# [<...,] typename T[, ...>]    TEMPLATE_TYPE_PARAMETER
# template<...> using T = ...;  TYPE_ALIAS_TEMPLATE_DECL
# using T = ...;                TYPE_ALIAS_DECL
# void f(...) const override;   CXX_METHOD
# void f(const T&);             PARM_DECL 
#        ^^^^^^^^
# private:                      CXX_ACCESS_SPEC_DECL

class Static:
    """ Class for clang utilities.

    This class stores the filename of the preprocessed header file (see
    `mocker.undef_macros(...)`, which the `get_children` method needs
    access to.

    :ivar str filename: Unique identifier of temporary file.
    """
    filename: str = ""  # Unique identifier of temporary file.
    std: str = ""  # C++ version.
    compiler_flags: List[str] = []  # Additional compiler flags.
    prefix: str = ""  # Include prefix for headers of drmock.
    
    @staticmethod
    def get_children(root: clang.cindex.Cursor) -> Generator[clang.cindex.Cursor]:
        """ Return the children of `root` contained in the target file `filename`. """
        return (
            x for x in root.get_children() 
            if str(x.location.file) == Static.filename
        )

    @staticmethod
    def is_class(cursor: clang.cindex.Cursor) -> bool:
        """ Check if `cursor` is a class or class template. """
        return cursor.kind in {
            CursorKind.CLASS_DECL,
            CursorKind.CLASS_TEMPLATE
        }

    @staticmethod
    def is_type_alias_decl(cursor: clang.cindex.Cursor) -> bool:
        """ Check if `cursor` is a type alias declaration.

        .. note:
            
            Beware! Does not check for type alias _template_ declarations.
        """
        return (cursor.kind == CursorKind.TYPE_ALIAS_DECL)

    @staticmethod
    def is_parameter_decl(cursor: clang.index.Cursor) -> bool:
        """ Check if `cursor` is a parameter declaration. """
        return (cursor.kind == CursorKind.PARM_DECL)

class ExplicitInstantiation:
    """ Class for explicit instantiation declarations and definitions of
    C++ class templates.
    """
    def __init__(self, cls: str):
        self.cls = cls

    def declaration(self) -> str:
        return "extern template class " + self.cls + ";"

    def definition(self) -> str:
        return "template class " + self.cls + ";"

class Overload:
    """ Class for a set of overloaded C++ methods.

    :ivar List[Method] methods: List of `Method`s of the overload.
    """

    def __init__(self, methods: List[Method]):
        self.methods = methods

    def is_proper(self) -> bool:
        """ Check if the overload holds more than one function. """
        return (len(self.methods) > 1)

    def make_getter(self) -> Method:
        """ Return the overload's mock method getter template. """
        f = self.methods[0]  # Representative of the overload.
        getter = Method()
        getter.template = TemplateDeclaration("... DRMOCK_INNER_Ts")
        getter.name = f.mangled_name()
        getter.return_type = Type.from_spelling("auto &")
        # The dispatch method is called by the getter. Construct the
        # arguments of the function call...
        dispatch_args = TemplateArguments(
            Type.from_spelling("DRMOCK_INNER_Ts ...")
        )
        # If all methods have the same parameter types, these must
        # automatically be passed as arguments to the dispatch method.
        if all(f.parameters == g.parameters for g in self.methods):
            dispatch_args.args.extend(f.parameters)
        # If all overloads are const qualified, the const qualifier must
        # automatically be passed as arguments to the dispatch method.
        if all(g.is_const for g in self.methods):
            dispatch_args.args.append(
                Type.from_spelling("drmock::Const")
            )
        getter.body = MethodBody(
            "return "
            + f.mangled_name()
            + "_dispatch(TypeContainer"
            + str(dispatch_args)
            + "{});"
        )
        return getter

    def make_pointers(self) -> List[MemberVariable]:
        """ Return the overload's `std::shared_ptr<Method>` objects. 

        For each function of the overload a `MemberVariable` object
        (whose `type` is `std::shared_ptr<Method>`) is created. This
        shared pointer represents the function in the mock object. Their
        names are `DRMOCK_[method name]_[integer]`. For example:

        DRMOCK_method_0
        DRMOCK_method_1
        DRMOCK_method_2

        The `MemberVariable`s are numbered in the order in which they
        occur in `self.methods`.

        .. note:

            (See also: make_dispatch) The return values of
            `Overload.make_pointers` and `Overload.make_dispatch` must
            be ordered in the same way in relation to `self.methods`.
            In other words, if `overload` is an instance of `Overload`,
            then `overload.make_pointers()[i]` and
            `overload.make_dispatch()[i]` must correspond to the same
            element of `overload.methods`. 
        """
        result = []
        i = 0  # Index for numbering overloads...
        for f in self.methods:
            # Create a template from the signature of `f`. The types
            # must be decayed before being introduced.
            template_args = TemplateArguments(
                f.return_type, 
                *(param.decayed() for param in f.parameters)
            )
            # Polymorphic method object.
            value_type = "Method" + str(template_args)
            shared_ptr = MemberVariable(
                name = "METHODS_DRMOCK_" + f.mangled_name() + "_" + str(i),
                type = Type.from_spelling("std::shared_ptr<" + value_type + ">"),
                constructor_args = ["std::make_shared<" + value_type + '>("", STATEOBJECT_DRMOCK_)']
            )
            # Append to the private members of `mo` and to the list of identifiers
            # later to be used to initialize the `MethodCollection` object.    
            result.append(shared_ptr)
            i += 1
        return result
    
    def make_dispatch(self) -> List[Method]:
        """ Return the overload's dispatch method objects. 

        For each function of the overload a `Method` (C++) object is
        created. These dispatch method take different combinations of
        specializations of C++ `TypeContainer`s as arguments and,
        depending on the specialization, return one of the
        `std::shared_ptr<Method>`s members. 
        
        The `Methods`s are ordered in the same order in which the
        methods they are construction from occur in `self.methods`.
            
        .. note:

            (See also: make_pointers) The return values of
            `Overload.make_pointers` and `Overload.make_dispatch` must
            be ordered in the same way in relation to `self.methods`.
            In other words, if `overload` is an instance of `Overload`,
            then `overload.make_pointers()[i]` and
            `overload.make_dispatch()[i]` must correspond to the same
            element of `overload.methods`. 

        .. note: 

            The dispatch method's name is the mangled name of the
            original method, concatenatied by `"_dispatch"`. This is
            necessary for writing dispatch methods for operators.
        """
        result = []
        i = 0  # Index for numbering overloads...
        for f in self.methods:
            dispatch = Method()
            dispatch.name = f.mangled_name() + "_dispatch"
            dispatch.return_type = Type.from_spelling("auto&")
            # The mocked method's cv qualifiers are stored in
            # the`TypeContainer`'s template arguments, together the
            # parameters of `f`.
            params_with_cv = copy(f.parameters)  # Copy, otherwise `f` is modified!
            if f.is_const:
                params_with_cv.append(Type.from_spelling("drmock::Const"))
            template = TemplateArguments(*params_with_cv)
            dispatch.parameters = [
                Type.from_spelling("TypeContainer" + str(template))
            ]
            dispatch.body = MethodBody(
                "return *" + "METHODS_DRMOCK_" + f.mangled_name() + "_" + str(i) + ";"
            )
            result.append(dispatch)
            i += 1
        return result

    def make_implementation(self) -> List[Method]:
        """ Return the mock implementation of the overload. 

        For every element of `self.methods`, return a `Method` object
        that calls the method getter function template. The template
        arguments are the parameters of the mocked method, and its cv
        qualifiers. If the original method is non-void, then the mock
        implementation forwards the return value of mock object's getter
        function template.
        """
        result = []
        for f in self.methods:
            f_impl = deepcopy(f)  # Parameters, cv qualifiers, etc. remain the same.
            f_impl.is_pure_virtual = False
            f_impl.is_override = True
            # Set variable names to a0, a1, a2, ...
            variables = tuple("a" + str(i) for i in range(0, f.num_parameters()))
            f_impl.set_variables(*variables)
            # Create a template from the parameter list of `f`.
            template_args = TemplateArguments(*f.parameters)
            # If `f` is const, add the const qualifier to the template
            # arguments.
            if f_impl.is_const:
                template_args.args.append(Type.from_spelling("drmock::Const"))
            # If `self` is a proper overload (holds multiple methods),
            # pass the template arguments.
            if self.is_proper():
                call = "mock.template " + f_impl.mangled_name() + str(template_args) + "().call(" 
                call += ", ".join(var.unpack() for var in f_impl.parameters) + ");"
            # If `self` is not proper, the correct template arguments
            # are automatically used, and need not be manually inserted.
            else:
                call = "mock." \
                    + f_impl.mangled_name() \
                    + "().call(" \
                    + ", ".join(var.unpack() for var in f_impl.parameters) \
                    + ");"
            # If `f` is void, there's nothing to do except set the
            # method body.
            if f.return_type == Type.from_spelling("void"):
                f_impl.body = MethodBody(call)
            # If `f` is non-void, then the result of the call must be
            # forwarded.
            else: 
                f_impl.body = MethodBody(
                    "auto& result = *" + call,
                    "return std::forward<" \
                        + str(f.return_type) \
                        + ">(drmock::moveIfNotCopyConstructible(result));"
                )  
            result.append(f_impl)
        return result

class EmptyLine:
    """ Class for empty lines in a `Class` or `CppFile` object. """

    def __init__(self):
        pass

    def __str__(self):
        return ""

    def draw(self, tab) -> str:
        return ""

@dataclass
class CppFile:
    """ Class for C++ header files. 

    :ivar str filename: The header's filename.
    :ivar str statements: List of C++ statements occuring in the header.
    """
    filename: str = ""
    statements: list = field(default_factory = list)
    tu: Union[NoneType, clang.cindex.TranslationUnit] = None

    @staticmethod
    def from_unit(tu: clang.cindex.TranslationUnit) -> CppFile:
        # Create header object and set translation unit.
        header = CppFile()
        header.tu = tu
        return header

    def get_class(self, regex: str) -> Class:
        """ Create a class object from the first class declaration whose
        name matches `regex`. If there are none, return `None`.

        :param tu: The translation unit.
        :type tu: `clang.cindex.TranslationUnit`.
        :returns: The constructed `CppFile`.
        :rtype CppFile:
        :raises: `ValueError` if `tu` holds multiple class declarations.
        """ 
        cls = None
        def visit_tree(
            cursor: clang.cindex.Cursor, 
            namespace: List[str] = []
        ) -> Class:
            """ Find the first class whose name matches `regex`.

            :param cursor: The root of the tree.
            :type cursor: clang.cindex.Cursor.
            :param namespace: Stores the namespace that `cursor` is enclosed in.
            :type namespace: list
            """
            nonlocal cls
            for x in Static.get_children(cursor):
                if x.kind == CursorKind.NAMESPACE:
                    # Add namespace upon entering the node.
                    namespace.append(x.displayname)  
                    visit_tree(x, namespace)  
                    # Remove the namespace upon leaving the node.
                    namespace.pop()
                elif Static.is_class(x):
                    name = x.spelling
                    if re.match(regex, name):
                        # Create a class and push the enclosing namespace 
                        cls = Class.from_cursor(x, copy(namespace))
                        break
        visit_tree(self.tu.cursor)
        return cls

    def __str__(self):
        return "\n".join(str(decl) for decl in self.statements)

class AccessSpecifier:
    """ Class for C++ access specifier declarations. 

    :ivar str _specicier: The access specifier. May be `"public"` or
        `"private"`.
    """

    def __init__(self, specifier: str):
        """ 
        :raises ValueError: if `specifier` is not `"public"` or `"private"`
        """
        if specifier not in {"public", "private", "signals"}:
            raise ValueError(f"invalid access specifier '{specifier}'")
        self._specifier = specifier
    
    @property
    def specifier(self) -> str:
        return self._specifier

    @staticmethod
    def from_cursor(cursor: clang.cindex.Cursor) -> AccessSpecifier:
        """ Create an `AccessSpecifier` a python clang cursor. 
        
        :raises ValueError: if `cursor.kind` is not equal to
            `CursorKind.ACCESS_SPEC_DECL`
        """
        if cursor.kind != CursorKind.CXX_ACCESS_SPEC_DECL:
            raise ValueError(
                f"invalid CursorKind. Expected: CursorKind.ACCESS_SPEC_DECL; received: {cursor.kind}"
            )
        tokens = list(cursor.get_tokens())
        if tokens == []:
            t = "signals"
        else:
            t = str(tokens[0].spelling)
        return AccessSpecifier(t)
    
    def __str__(self):
        return self.specifier + ":"

    def draw(self, tab) -> str:
        return min(0, tab - 1)*"  " + str(self)

@dataclass
class IncludeDirective:
    """ Class for C++ include directives. """
    filename: str

    def __str__(self):
        return '#include "' + self.filename + '"'

    def draw(self, tab) -> str:
        return tab*"  " + str(self)

@dataclass
class UsingDirective:
    """ Class for C++ using directives. """
    definition: str

    @staticmethod
    def from_cursor(cursor: clang.cindex.Cursor) -> UsingDirective:
        """ Create a `UsingDirective` a python clang cursor. 
        
        :raises ValueError: if `cursor.kind` is not equal to `CursorKind.USING_DIRECTIVE`.
        """
        if cursor.kind != CursorKind.USING_DIRECTIVE:
            raise ValueError(
                f"invalid CursorKind. Expected: CursorKind.USING_DIRECTIVE; received: {cursor.kind}"
            )
        return UsingDirective(cursor.get_definition().spelling)

    def __str__(self):
        return "using namespace " + self.definition + ";"

    def draw(self, tab) -> str:
        return tab*"  " + str(self)

@dataclass
class Type:
    """ Class for C++ types.

    Types involving cv qualifiers, pointers or references (like `const T*`)
    are represented recursively. The member `inner` holds a `str` or a
    `Type`, while the other members store the properties of the outer
    type.

    .. note::
        
        Although they are implemented in `Type`, volatile qualifiers are
        currently not supported by `mocker.py`.

    .. note::
        
        Although references are always `const`, this is not reflected in
        the value of `is_const`. 

    .. note::
        
        `const` qualified references (that is: a reference that is
        `const`, not a reference to a `const` qualified object)
        should not be used at all. (All references in C++ are
        constant.) Ignoring this may result in unwanted behavior.
        For instance, printing a `Type` that represents a const
        reference to a const object results in a duplicate const
        qualifier. The same goes for volatile qualifiers.

    .. example:: 

        ```
        type0 = Type.from_spelling("const T *")  # This is a `Type`.
        print(type0)  # const T*
        type1 = type0.inner  # This is a `Type`.
        print(type1)  # const T
        type2 = type1.inner  # This a `str`, `"T"`.
        print(type2)  # T
        ```
    """
    inner: Union[str, Type] = "void"
    is_const: bool = False
    is_volatile: bool = False
    is_lvalue_ref: bool = False
    is_rvalue_ref: bool = False
    is_pointer: bool = False
    is_parameter_pack: bool = False

    def center(self) -> str:
        """ Return the innermost part of the type. """
        type = self
        while (not isinstance(type.inner, str)):
            type = type.inner
        return type.inner

    def is_naked(self):
        """ Check if `self` is non-cv-qualified, non-reference,
        non-pointer, non-parameter-pack. """
        return (not self.is_const
            and not self.is_volatile
            and not self.is_lvalue_ref
            and not self.is_rvalue_ref
            and not self.is_pointer
            and not self.is_parameter_pack
        )

    def purge(self) -> void:
        """ Purge naked inner types from `self`. """
        # Find the outer-most non-naked type in the hierarchy. This type
        # will form the outer type of the result.
        type = self
        while not isinstance(type.inner, str) and type.is_naked():
            type = type.inner
        # If this outer type is a base-type (naked with `str` as inner
        # type), there is nothing left to do:
        if isinstance(type.inner, str):
            return type
        # Otherwise, purge the inner type.
        type.inner = type.inner.purge()
        return type

    def decayed(self) -> Type:
        """ Return the decayed type of `self`. """
        # Note that if an instance of `Type` is a reference, then its
        # const qualifier is saved in the inner type. This is due to the
        # confusion between the terms "const reference" and "reference
        # to const": The distinction between "const reference" and
        # "reference to const" is unnecessary, and the former is only a
        # shorthand for the latter. The same goes for volatile
        # qualifiers. 
        #
        # Therefore, when decaying a reference, the cv qualifiers must
        # be removed from the inner type; otherwise, they must be
        # removed from the instance itself.
        type = deepcopy(self)
        if type.is_lvalue_ref or type.is_rvalue_ref:
            type.is_lvalue_ref = False
            type.is_rvalue_ref = False
            type.inner.is_const = False
            type.inner.is_volatile = False
        else:
            type.is_const = False
            type.is_volatile = False
        # If `type` used to be a (const) reference, it is now naked and
        # must be purged.
        return type.purge()
    
    @staticmethod
    def from_cursor(cursor: clang.cindex.Cursor) -> Type:
        """ Create a `Type` a python clang cursor. 
        
        :raises ValueError: if `cursor.kind` is not equal to `CursorKind.PARM_DECL`.
        """
        if cursor.kind != CursorKind.PARM_DECL:
            raise ValueError(
                f"invalid CursorKind. Expected: CursorKind.PARM_DECL; received: {cursor.kind}"
            )

        # The following is a hack to solve some rather unfortunate
        # behavior of python clang. If a type alias such as
        # 
        # using T = std::shared_ptr<int>;
        #
        # is used in a class, say outer::inner::Foo, then it might
        # happen that in a function declaration such as
        #
        # void f(T);
        #
        # `T` is expanded into `inner::outer::Foo`, unless `T` is
        # suffciently buried (for instance, `std::shared_ptr<T>` will
        # not be expanded). This seems to happen with `spelling`,
        # `type.spelling`, `displayname`, etc. 
        # 
        # Leaving this unchanged would render the `Method` object
        # corresponding to this declaration dependent on the class that
        # the declaration occured in.  But we may want to move the
        # method (and the type alias) into another class!
        # 
        # Another matter is that, even if `Foo` is a class template, the
        # expanded name will not contain the template parameters
        # (`outer::inner::Foo` instead of `outer::inner::Foo<...>`).
        # Therefore, parsing and printing a class template with type
        # aliases will result in code that will raise a compiler error.
        #
        # The problem is solved by taking the tokens of the parameter
        # declarations and joining them into a string. 
         
        # But two related problems arise when using tokens. If a
        # variable name is given to a parameter, as in 
        #
        # f(const T & ... foo);
        #                 ^^^
        #
        # then that name will appear in the tokens. It must therefore be
        # removed from the tokens. But beware! If no variable name or
        # "..." is present in the parameter declaration, then an outer
        # `const` as in
        #
        # f(const T* const);
        #            ^^^^^
        #
        # is lost when considering the cursor's tokens, and thus
        # `from_spelling` cannot be used. Fortunately, this outer const
        # qualifier can be recognized using clang python. Thus, in the
        # case of a parameter pack or a parameter decl with variable
        # name, `from_spelling` may be called after popping the variable
        # name. Otherwise, a complex route must be taken.

        # Check for variable name or parameter pack and call `from_spelling`.
        spells = [t.spelling for t in cursor.get_tokens()]  # ["const", "T", "&", "...", "foo"]
        var = cursor.spelling  # "foo"
        if var != "" and spells[-1] == var:
            spells.pop()
            return Type.from_spelling(" ".join(spells))
        elif spells[-1] == "...":
            return Type.from_spelling(" ".join(spells))
        # If both tests fail, check for const qualifier using python clang.
        type = Type.from_spelling(" ".join(spells))
        type.is_const = cursor.type.is_const_qualified()
        type.is_volatile = cursor.type.is_volatile_qualified()
        return type

    @staticmethod
    def from_spelling(spell) -> Type:
        """ Recursively create a `Type` object from a python clang
        spelling.

        This function manually parses and disassembles the spelling
        manually, as `clang.cindex.Type` does not seem to have any means
        of checking if the represented C++ type is an lvalue reference,
        an rvalue reference, a pointer or a parameter pack.

        :param spell: A tokenized string.
        :type spell: str
        :returns: The string represented by `spell`.
        :rtype Type:

        .. note::

            _Tokenized_ in this context means that all tokens of the
            type declaration are well-seperated in `spell`. For
            instance, "T &" is allowed, "T&" is not.
        """
        # TODO If, in the future, clang.cindex.Type is equipped with
        # methods to check if the represented type is an lvalue
        # reference, etc., then this function should be simplified using
        # said methods.

        t = Type()
        tokens = spell.split(" ")
        # Read from the right.
        while True:
            if tokens[-1] == "const":
                t.is_const = True
                tokens.pop()
            elif tokens[-1] == "volatile":
                t.is_volatile = True
                tokens.pop()
            elif tokens[-1] == "...":
                t.is_parameter_pack = True
                tokens.pop()
            elif tokens[-1] == "*":
                t.is_pointer = True
                tokens.pop()
                break
            elif tokens[-1] == "&":
                t.is_lvalue_ref = True
                tokens.pop()
                break
            elif tokens[-1] == "&&":
                t.is_rvalue_ref = True
                tokens.pop()
                break
            else:
                break
        # If `t` is a pointer or a reference, reassemble the remaining
        # tokens and call `from_spelling` recursively.
        if t.is_pointer or t.is_lvalue_ref or t.is_rvalue_ref:
            t.inner = Type.from_spelling(" ".join(tokens))
            return t
        # If `t` is not a pointer or a reference, then read from the
        # left.
        while True:
            if tokens[0] == "const":
                t.is_const = True
                tokens.pop(0)
            elif tokens[0] == "volatile":
                t.is_volatile = True
                tokens.pop(0)
            else:
                break
        # Terminate the recursion by reassembleing the remaining tokens.
        t.inner = " ".join(tokens)
        return t
    
    def __eq__(self, other):
        if not isinstance(other, Type):
            return NotImplemented
        return (self.inner == other.inner
            and self.is_const == other.is_const
            and self.is_volatile == other.is_volatile
            and self.is_lvalue_ref == other.is_lvalue_ref
            and self.is_rvalue_ref == other.is_rvalue_ref
            and self.is_pointer == other.is_pointer
            and self.is_parameter_pack == other.is_parameter_pack
        )

    def __str__(self):
        s = ""
        s += str(self.inner)
        s += self.is_pointer*" *"
        s += self.is_lvalue_ref*" &"
        s += self.is_rvalue_ref*" &&"
        if self.is_pointer:
            s += self.is_const*" const"
            s += self.is_volatile*" volatile"
        else:
            s = self.is_const*"const " + self.is_volatile*"volatile " + s
        s += self.is_parameter_pack*" ..."
        return s

    def is_ref(self) -> bool:
        """ Return `True` if `self` is a lvalue or rvalue reference. """
        return (self.is_lvalue_ref or self.is_rvalue_ref)

class TemplateArguments:
    """ Class that represents a list of C++ template arguments `<T1, ..., Tn>`. 

    :ivar List[Type] _args: The template arguments `T1, ... Tn`
    """

    def __init__(self, *args: Tuple[Type]):
        """ 
        :param args: The template arguments.
        :type args: Tuple[Type]
        """
        self._args = list(args)

    @property
    def args(self) -> List[Type]:
        return self._args

    def __str__(self):
        return "<" + ", ".join(str(t) for t in self.args) + ">"

class TemplateDeclaration:
    """ Class for C++ template declarations `template <T1, ..., Tn>`. 

    :ivar List[str] _parameters: (The names of) the template parameters 
        `T1, ..., Tn`.
    """

    def __init__(self, *args: Tuple[str]):
        """
        :param args: The template parameters.
        :type args: Tuple[str]
        """
        self._parameters = list(args)

    @property
    def parameters(self) -> List[Type]:
        return self._parameters
    
    @staticmethod
    def from_cursor(cursor: clang.cindex.Cursor) -> TemplateDeclaration:
        """ Create a `TemplateDeclaration` from the children of `cursor`. """
        template = TemplateDeclaration()
        for x in Static.get_children(cursor):
            if x.kind == CursorKind.TEMPLATE_TYPE_PARAMETER:
                # The following is a hack used to circumvent the problem
                # that `cindex.CursorKind.TEMPLATE_TYPE_PARAMETER` is
                # unable to recognize variadic template parameters.
                # Fortunately, it is possible to detect "..." using the
                # `get_tokens()` method.
                tokens = [t.spelling for t in x.get_tokens()]  # ["typename", "T"] or ["typename", "...", "Ts"]
                name = " ".join(tokens[1:])  # "T" or "... Ts"
                template.parameters.append(name)
        return template
                
    def __str__(self):
        return "template<" + ", ".join("typename " + str(s) for s in self.parameters) + ">"
       
    def to_args(self) -> TemplateArguments:
        """ Return `TemplateArguments` whose args match the parameters of `self`. 

        .. example:: 
            
            temp_decl = TemplateDeclaration("T", "... Ts")
            temp_args = temp_decl.to_args()
            print(temp_args)  # "<T, Ts...>"
        """
        # Replace all variadic template parameters "... Ts" with "Ts ...".
        args = [
            utils.swap("\.\.\. (.*)", "\\1 ...", s) 
            if s.startswith("...") else s for s in self.parameters
        ]
        # Create types from the argument strings.
        args = [Type.from_spelling(arg) for arg in args]
        return TemplateArguments(*args)

@dataclass
class TypeAlias:
    """ Class for C++ non-template type aliases. """
    name: str 
    typedef: Type

    @staticmethod
    def from_cursor(cursor: clang.cindex.Cursor) -> TypeAlias:
        """ Create a `TypeAlias` from the children of `cursor`. 

        :raises ValueError: if `cursor.kind` is not `CursorKind.TYPE_ALIAS_DECL`.
        """
        if cursor.kind != CursorKind.TYPE_ALIAS_DECL:
            raise ValueError(
                f"invalid CursorKind. Expected: CursorKind.TYPE_ALIAS_DECL; received: {cursor.kind}"
            )
        return TypeAlias(
            name = cursor.spelling, 
            typedef = cursor.underlying_typedef_type.spelling
        )

    def __str__(self):
        return "using " + self.name + " = " + str(self.typedef) + ";"
    
    def draw(self, tab) -> str:
        return tab*"  " + str(self)

@dataclass 
class TypeAliasTemplate:
    """ Class for C++ type alias templates. """
    type_alias: TypeAlias
    template: TemplateDeclaration

    @staticmethod
    def from_cursor(cursor: clang.cindex.Cursor) -> TypeAliasTemplate:
        """ Create a `TypeAliasTemplate` from `cursor`. 

        :raises ValueError: if `cursor.kind` is not
            `CursorKind.TYPE_ALIAS_TEMPLATE_DECL`.
        """
        if cursor.kind != CursorKind.TYPE_ALIAS_TEMPLATE_DECL:
            raise ValueError(
                f"invalid CursorKind. Expected: CursorKind.TYPE_ALIAS_TEMPLATE_DECL; "  
                "received: {cursor.kind}"
            )
        return TypeAliasTemplate(
            TypeAlias.from_cursor(
                next(
                    x for x in Static.get_children(cursor) if Static.is_type_alias_decl(x)
                )
            ),
            TemplateDeclaration.from_cursor(cursor)
        )

    def __str__(self):
        return str(self.template) + " " + str(self.type_alias)
    
    def draw(self, tab) -> str:
        s = ""
        s += tab*"  " + str(self.template) + "\n" 
        s += tab*"  " + str(self.type_alias)
        return s

@dataclass
class FriendClass:
    """ Class for C++ friend class declarations. """
    friend: Type

    def __str__(self):
        return "friend class " + str(self.friend) + ";"

    def draw(self, tab) -> str:
        return tab*"  " + str(self)

@dataclass
class Class:
    """ Class for C++ class declarations or definitions. 

    :ivar str name: The class' name.
    :ivar List[str] namespace: The class' enclosing namespace.
    :ivar list private: The class' private declarations.
    :ivar list public: The class' public declarations.
    :ivar bool is_final: `True` if and only if the class has the `final`
        keyword.
    :ivar bool is_qobject: `True` if and only if the `Q_OBJECT` macro
        occurred in the class' definition. 
    :ivar Union[NoneType, Type] inherits_from: The class' parent; `None`
        if the class doesn't inherit.
    :ivar Union[NoneType, TemplateDeclaration] template: The class
        template parameters; `None` if not a template.
    """
    name: str = "T"
    namespace: List[str] = field(default_factory=list)
    private: list = field(default_factory=list)
    public: list = field(default_factory=list)
    is_final: bool = False
    is_qobject: bool = False
    inherits_from: List[Union[Type, str]] = field(default_factory=list)
    template: Union[NoneType, TemplateDeclaration] = None
    
    def abstract_methods(self) -> List[Method]:
        abstract_methods = [
            f for f in self.public if 
            isinstance(f, Method) and f.is_pure_virtual
        ]
        return abstract_methods

    def is_explicit(self) -> bool:
        return (self.template is None and not self.contains_typedef())

    def contains_typedef(self) -> bool:
        """ Return if any (template) using declarations occur in `self`.
        """
        return any(
            isinstance(decl, TypeAlias) or isinstance(decl, TypeAliasTemplate)
            for decl in self.public
        ) or any(
            isinstance(decl, TypeAlias) or isinstance(decl, TypeAliasTemplate)
            for decl in self.private
        )

    def full_name(self) -> str:
        """ Returns the class' name with prefixed enclosing namespace.

        .. example:: 

            cls = Class(name = "T", namespace = ["outer", "inner"]) 
            cls.full_name()  # "outer::inner::IExample" 
        """
        s = ""
        s += "".join(space + "::" for space in self.namespace)
        s += self.name
        return s

    def is_derived(self) -> bool:
        """ Check if `self` represents a derived class. """
        return (self.inherits_from != [])

    def get_overloads(self) -> List[Overload]:
        abstract_methods = [
            f for f in self.public if 
            isinstance(f, Method) and f.is_pure_virtual
        ]
        split_ = utils.split(
            abstract_methods, 
            lambda f: f.mangled_name()
        )
        return [Overload(sublist) for sublist in split_]

    @staticmethod 
    def from_cursor(cursor: clang.cindex.Cursor, namespace: List[str]) -> Class:
        """ Initialize a `Class` object in namespace `namespace` from
        `cursor`.

        :raises ValueError: if `cursor.kind` is not
            `CursorKind.CLASS_DECL` or `CursorKind.CLASS_TEMPLATE`.
            
        :raises ValueError: if `cursor` points to a class that does not
            satisfy the definition of _interface_ given above.
        """
        if not Static.is_class(cursor):
            raise ValueError(
                f"invalid CursorKind. "
                "Expected: CursorKind.CLASS_DECL or CursorKind.CLASS_TEMPLATE; "
                "received: {cursor.kind}"
            )
        cls = Class()
        cls.name = cursor.spelling  
        cls.namespace = namespace
        # Add template parameters if the cursor points to a class template.
        if cursor.kind == CursorKind.CLASS_TEMPLATE:
            cls.template = TemplateDeclaration.from_cursor(cursor)
        # Create methods, type aliases (templates), using directives
        # from the children of `cursor`.
        access = AccessSpecifier("private")
        for x in Static.get_children(cursor): 
            # Ignore the following...
            if x.kind in {
                CursorKind.TEMPLATE_TYPE_PARAMETER,
                CursorKind.DESTRUCTOR
            }:
                continue
            # Catch inheritance.
            if x.kind == CursorKind.CXX_BASE_SPECIFIER:
                tokens = (t.spelling for t in x.get_tokens())
                access = next(tokens)
                assert(access == "public")
                cls.inherits_from.append(" ".join(tokens))
                continue
            # A field declaration can mean one of two things: (1)
            # Q_OBJECT macro created by mocker.py's preprocessor, (2) a
            # violation of the thou-shalt-not's in the docstring of this
            # file.
            if x.kind == CursorKind.FIELD_DECL:
                tokens = (t.spelling for t in x.get_tokens())  # int Q_OBJECT
                next(tokens)  # Pop the type of the declaration.
                var = next(tokens, None)  # Get the variable name.
                if var == "Q_OBJECT":
                    cls.is_qobject = True
                    continue
                # If (2) is the case, below `type` will be set to None
                # and a ValueError will be raised...
            # Get the cursor type of `x`.
            supported = {
                CursorKind.CXX_METHOD: Method,
                CursorKind.USING_DIRECTIVE: UsingDirective,
                CursorKind.TYPE_ALIAS_TEMPLATE_DECL: TypeAliasTemplate,
                CursorKind.TYPE_ALIAS_DECL: TypeAlias,
                CursorKind.CXX_ACCESS_SPEC_DECL: AccessSpecifier
            }
            type = supported.get(x.kind, None)
            if type == AccessSpecifier:
                # Raise if a private access specifier occurs.
                access = AccessSpecifier.from_cursor(x)
                if access.specifier == "private":
                    raise ValueError(
                        f"encountered unsupported private access specifier in class {cls.name}"
                    )
                # If it's "public" or "signals" instead, ignore.
                continue
            # Raise if an unsupported cursor occurs.
            if type is None:
                raise ValueError(f"encountered unsupported cursor {x.kind} in class {cls.name}")
            # If all checks pass, create (public) methods, type alias
            # (templates), using directives. Ignore the content if they
            # are private.
            if access.specifier == "public":
                cls.public.append(type.from_cursor(x))
        return cls

    def __str__(self):
        s = ""
        # Namespace open braces.
        if self.namespace:
            s += " ".join("namespace " + ns + " {" for ns in self.namespace)
            s += "\n\n"
        # Template delcaration.
        if self.template:
            s += str(self.template) + "\n"
        # Class declaration.
        s += "class " + self.name + self.is_final*" final"
        if self.is_derived():
            s += " : " + ", ".join(
                "public " + str(base) for base in self.inherits_from
            )
        s += "\n"
        s += "{"  # Body of class declaration
        s += "\n"
        # Q_OBJECT macro.
        if self.is_qobject:
            s += "  Q_OBJECT\n"
            s += "\n"
        # Private delcarations.
        if self.private:
            s += AccessSpecifier("private").draw(1) + "\n"
            s += "".join(decl.draw(1) + "\n" for decl in self.private)
        # Public declarations.
        if self.public:
            s += "\n"
            s += AccessSpecifier("public").draw(1) + "\n"
            s += "".join(decl.draw(1) + "\n" for decl in self.public)
        # Body of class declaration ends.
        s += "};"  
        # Namespace close braces.
        if self.namespace:
            s += "\n\n"
            s += len(self.namespace)*"}" 
            s += " // namespace "
            s += "::".join(self.namespace)
        return s

    def mock(
        self, 
        interface_file: str,  # Input file
        mock_name: str,  # Mock name
        mock_file: str
    ) -> Tuple[CppFile]:
        """ Create two `CppFile`s that holds a mock object's and a mock
        declaration and implementation constructed from `self`.

        :raises ValueError: if `self.statements` does not hold exactly
            one object of type `Class`.
        """
        
        # Create a header file, mock object and mock implementation.
        hdr = CppFile()
        mock_object, mock_implementation = self.make_mock(
            mock_name
        )
        # Open the include guard. The include guard string is obtained
        # by replacing all dots and slashes with underscores to make the
        # path readable. All remaining chars except [a-zA-Z0-9_] are
        # replaced with `"_DRMOCK_UNSUPPORTED_CHAR_"`.
        include_guard = "DRMOCK_MOCK_IMPLEMENTATIONS_GENERATED_FROM" \
            + interface_file
        include_guard = include_guard.replace(".", "_") \
            .replace("/", "_") \
            .upper()
        include_guard = re.sub(
            "[^a-zA-Z0-9_]", 
            "DRMOCK_UNSUPPORED_CHAR",
            include_guard
        )
        include_guard = include_guard.replace("__", "_")  # Prevent double underscores.
        hdr.statements.append("#ifndef " + include_guard)
        hdr.statements.append("#define " + include_guard)
        hdr.statements.append(EmptyLine())
        # Append an input directive for the interface's header file, and the
        # required headers from drmock.
        hdr.statements.append(IncludeDirective(interface_file))
        hdr.statements.append(IncludeDirective(Static.prefix + "Mock.h"))
        hdr.statements.append(EmptyLine())
        # If `self` is not a template class, then add the declarations
        # of the explicit instantiations of `Method`.
        if self.is_explicit():
            explicit = self._make_explicit()
            hdr.statements.extend([
                e.declaration() for e in explicit
            ])
            hdr.statements.append(EmptyLine())
        # Append the class declarations, seperated by an empty line.
        hdr.statements.append(mock_object)
        hdr.statements.append(EmptyLine())
        hdr.statements.append(mock_implementation)
        hdr.statements.append(EmptyLine())
        # Close the include guard.
        hdr.statements.append("#endif /* " + include_guard + " */")

        # If `self` is not a template class, create a source file
        # containing the definitions of the explicit instantiations.
        # Otherwise, return a source file that is empty safe for a
        # comment that prevents the AutoGen "empty file" warning..
        src = CppFile()
        if self.is_explicit():
            src.statements.append(IncludeDirective(mock_file))
            src.statements.append(EmptyLine())
            src.statements.extend([
                e.definition() for e in explicit
            ])
        else:
            # Prevents AutoGen warning.
            src.statements.append("// Empty.")  
        return hdr, src

    def make_mock(
        self, 
        mock_name: str
    ) -> Tuple[Class]:
        """ Return the mock object and implementation of `self`. 

        See `_make_mock_object` and `_make_mock_implementation` for
        details.
        """
        mock_object = self._make_mock_object()
        mock_implementation = self._make_mock_implementation(
            mock_name, 
            mock_object
        )
        return mock_object, mock_implementation

    def _make_explicit(self) -> List[ExplicitInstantiation]:
        # Get the decayed signature of all methods. Throw away
        # duplicates.
        abstract_methods = [
            f for f in self.public if 
            isinstance(f, Method) and f.is_pure_virtual
        ]
        decayed = {
            (str(f.return_type), *(str(p.decayed()) for p in f.parameters))
            for f in abstract_methods
        }
        # From these signatures, create instantiations of `Method`.
        instantiations = {
            "drmock::Method<" + ", ".join(sig) + ">"
            for sig in decayed
        }
        return [ExplicitInstantiation(i) for i in instantiations]

    def _make_mock_object(
        self
    ) -> Class:
        """ Return a `Class` object that represents the mock object of `self`. """
        mo = Class()
        mo.name = "DRMOCK_Object_" + self.name
        mo.namespace = ["drmock", "mock_implementations"]
        # The internal mock object has the same template as the interface.
        mo.template = self.template
        # Add the interface's type alias declarations, but ignore using
        # directives. Push them to the top of the private declarations,
        # which are printed before the public declarations.
        type_aliases = [
            x for x in self.public
            if isinstance(x, TypeAlias) 
            or isinstance(x, TypeAliasTemplate) 
        ]
        mo.private.extend(type_aliases)

        # For every overload create a getter method.
        overloads = self.get_overloads()
        mo.public.extend([
            overload.make_getter() for overload in overloads
        ])

        # Create the shared StateObject.
        mo.private.append(
            MemberVariable(
                name = "STATEOBJECT_DRMOCK_",
                type = Type.from_spelling("std::shared_ptr<StateObject>"),
                constructor_args = ["std::make_shared<StateObject>()"]
            )
        )

        # Create the verifyState methods.
        mo.public.append(
            Method(
                name = "verifyState",
                return_type = Type.from_spelling("bool"),
                parameters = [Parameter("state", Type.from_spelling("const std::string&"))],
                body = MethodBody(
                    "return STATEOBJECT_DRMOCK_->get() == state;"
                )
            )
        )
        mo.public.append(
            Method(
                name = "verifyState",
                return_type = Type.from_spelling("bool"),
                parameters = [
                    Parameter("slot", Type.from_spelling("const std::string&")),
                    Parameter("state", Type.from_spelling("const std::string&"))
                ],
                body = MethodBody(
                    "return STATEOBJECT_DRMOCK_->get(slot) == state;"
                )
            )
        )

        # For every overload add the shared_ptr's to the Method objects. 
        pointers = []
        for overload in overloads:
            # Make the pointers, push them to identifiers.
            pointers.extend(overload.make_pointers())
        mo.private.extend(pointers)
        for overload in overloads:
            mo.private.extend(overload.make_dispatch())

        # Append a `MemberVariable` representing a cpp object of type
        # `MethodCollection` constructed from the
        # `std::shared_ptr<Method>`s contained in the mock object.
        ids = [p.name for p in pointers]
        collection = MemberVariable(
            name = "methods",
            type = "MethodCollection",
            constructor_args = [
                "std::vector<std::shared_ptr<IMethod>>{" + ", ".join(ids) + "}"
            ]
        )
        mo.private.append(collection)

        # Construct and append the associated `verify` method.
        verify = Method()
        verify.name = "verify"
        verify.return_type = Type.from_spelling("bool")
        verify.body = MethodBody("return " + collection.name + ".verify();")
        mo.public.append(verify)
        
        # Declare MockObject a friend of the mocked interface. Note that the
        # typename must be prefixed with the enclosing namespace.
        mock_impl_type = self.full_name() 
        if mo.template is not None:
            mock_impl_type += str(mo.template.to_args())
        friend_decl = FriendClass(Type.from_spelling(mock_impl_type))
        mo.private.append(friend_decl)
        return mo

    def _make_mock_implementation(
        self, 
        mock_name: str,
        mock_object: Class
    ) -> Class:
        """ Return the mock implementation of `interface`. """
        mock = Class()
        mock.name = mock_name
        # Namespace and template parameters are the same as that of the
        # interface.
        mock.namespace = self.namespace
        mock.template = self.template
        # Insert the `Q_OBJECT` macro if it occurs in the interface.
        mock.is_qobject = self.is_qobject
        # Make `mock` a final derived class of the interface. If the
        # interface is a class template, then use the same template
        # parameters.
        parent = self.name
        if self.template is not None:
            parent += str(self.template.to_args())
        mock.inherits_from.append(Type.from_spelling(parent))
        mock.is_final = True
        # Add the interface's type alias declarations, but ignore using
        # directives.
        type_aliases = [
            x for x in self.public
            if isinstance(x, TypeAlias) 
            or isinstance(x, TypeAliasTemplate) 
        ]
        mock.public.extend(type_aliases)
        # Add an instance of the internal mock object as private member to
        # `mock`. 
        mo_name = mock_object.full_name()
        if mock_object.template is not None:
            mo_name += str(mock_object.template.to_args())
        mo_member = MemberVariable(
            name = "mock",
            type = mo_name,
            is_mutable = True
        )
        mock.public.append(mo_member)
        for overload in self.get_overloads():
            mock.public.extend(overload.make_implementation())
        return mock

class MethodBody:
    """ Class for C++ method bodies. 

    :ivar List[str] _statements: The statements occurring in the method
        body.
    """ 

    def __init__(self, *args: Tuple[str]):
        """
        :param args: The statements found in the function body.
        :type args: Tuple[str]
        """
        self._statements = list(args) 

    @property
    def statements(self) -> List[str]:
        return self._statements

    def draw(self, tab) -> str:
        s = ""
        s += tab*"  " + "{\n"
        s += "\n".join((tab + 1)*"  " + statement for statement in self.statements)
        s += "\n"
        s += tab*"  " + "}"
        return s

@dataclass
class Parameter:
    """ Class for C++ variables, function parameters or template
    arguments. 

    :ivar str name: The parameter's name.
    :ivar Type type: The parameter's type.
    """ 
    name: str 
    type: Type

    def __str__(self):
        return str(self.type) + " " + self.name

    def unpack(self) -> str:
        """ Unpack and move the parameter.

        .. example::
            ```
            param = Parameter(
                "foo",
                Type.from_spelling("T && ...")
            )
            param.unpack()  # "std::move(foo)..."
            ```
        """
        if self.type.is_lvalue_ref:
            s = self.name 
        else:
            s = "std::move(" + self.name + ")" 
        s += self.type.is_parameter_pack*" ..."
        return s

@dataclass
class MemberVariable:
    """ Class for C++ member declarations. """
    name: str 
    type: Type
    constructor_args: List[str] = field(default_factory = list)
    is_mutable: bool = False

    def __str__(self):
        s = ""
        s += self.is_mutable*"mutable " 
        s += str(self.type) 
        s += " " 
        s += self.name 
        s += "{"
        s += ", ".join(self.constructor_args) 
        s += "};"
        return s

    def draw(self, tab) -> str:
        return tab*"  " + str(self)

@dataclass
class Method:
    """ Class for C++ methods. """
    name: str = "f"
    parameters: List[Parameter] = field(default_factory=list)
    template: TemplateDeclaration = None
    return_type: Type = field(default_factory=Type)
    is_const: bool = False
    is_pure_virtual: bool = False
    is_override: bool = False
    is_noexcept: bool = False
    is_volatile: bool = False
    is_operator: bool = False
    body: MethodBody = field(default_factory=MethodBody)

    def depends_on(self, expr: str) -> bool:
        """ Return if the return value or any of the parameters depend
        on `expr`.
        """
        # Check the return type.
        center = self.return_type
        pass

    def set_variables(self, *args: Tuple[str]) -> void:
        """ Add variable names to the parameters `self`. 

        :param args: The variable names
        :type args: Tuple[str]

        .. example:: `f(const T&, Ts&&...)` goes to `f(const T& args[0], Ts&&... args[1])`.
        """
        if len(args) != self.num_parameters():
            raise ValueError(
                f"Received: {len(args)} parameters; expected: {self.num_parameters()}")
        self.parameters = [Parameter(name, type) for name, type in zip(args, self.parameters)]

    def num_parameters(self) -> int:
        return len(self.parameters)

    def mangled_name(self) -> str:
        """ Return the method's name with operator symbols replaced. 

        Use this to turn the spelling of an operator declaration into a
        legal method name.
        """
        str = self.name
        for key, item in {
            "<=>": "SpaceShip",  
            "->*": "PointerToMember",  
            "co_await": "CoAwait"
        }.items():
            str = str.replace(key, item)
        for key, item in {
            "==": "Equal",
            "!=": "NotEqual",
            "<=": "LesserOrEqual",
            ">=": "GreaterOrEqual",
            "<<": "StreamLeft",
            ">>": "StreamRight",
            "&&": "And",
            "||": "Or",
            "++": "Increment",  
            "--": "Decrement",  
            "->": "Arrow",
        }.items():
            str = str.replace(key, item)
        for key, item in {
            "+": "Plus",
            "-": "Minus",
            "*": "Ast",
            "/": "Div",  
            "%": "Modulo",  
            "^": "Caret",  
            "&": "Amp",  
            "|": "Pipe",  
            "~": "Tilde",  
            "!": "Not",  
            "=": "Assign",
            "<": "Lesser",
            ">": "Greater",
            ",": "Comma",
            "()": "Call",
            "[]": "Brackets"
        }.items():
            str = str.replace(key, item)
        return str

    @staticmethod
    def from_cursor(cursor: clang.cindex.Cursor) -> Method:
        """ Initialize a `Method` object from a `clang.cindex.Cursor`.

        :raises ValueError: if `cursor.kind` is not
            `CursorKind.CLASS_DECL` or `CursorKind.CLASS_TEMPLATE`.
            
        :raises ValueError: if `cursor` points to a class that does not
            satisfy the definition of _interface_ given above.
        """
        if cursor.kind != CursorKind.CXX_METHOD:
            raise ValueError(
                f"invalid CursorKind. Expected: CursorKind.CXX_METHOD; received: {cursor.kind}"
            )
        f = Method()
        f.name = cursor.spelling
        if not cursor.is_pure_virtual_method():
            raise ValueError(f"method {f.name} is not pure virtual")
        # Create method object.
        f.parameters = [
            Type.from_cursor(x) for x in Static.get_children(cursor) 
            if Static.is_parameter_decl(x)
        ]
        # The following is a hack to solve some rather unfortunate
        # behavior of python clang. If a type alias such as
        # 
        # using T = std::shared_ptr<int>;
        #
        # is used in a class, say outer::inner::Foo, then it might
        # happen that in a function declaration such as
        #
        # void f(T);
        #
        # `T` is expanded into `inner::outer::Foo`, unless `T` is
        # suffciently buried (for instance, `std::shared_ptr<T>` will
        # not be expanded). This seems to happen with `spelling`,
        # `type.spelling`, `displayname`, etc. 
        # 
        # Leaving this unchanged would render the `Method` object
        # corresponding to this declaration dependent on the class that
        # the declaration occured in.  But we may want to move the
        # method (and the type alias) into another class!
        # 
        # Another matter is that, even if `Foo` is a class template, the
        # expanded name will not contain the template parameters
        # (`outer::inner::Foo` instead of `outer::inner::Foo<...>`).
        # Therefore, parsing and printing a class template with type
        # aliases will result in code that will raise a compiler error.
        #
        # `Method.from_cursor()` uses `cindex.Type.get_canonical()` to
        # assemble the return type. If `Foo` is a class template and a
        # template parameter of `Foo` appears as parameter in the type
        # alias, it is represented in the form
        # 
        # shared_ptr<type-parameter-0-i>
        #
        # where `i` is the index of the parameter (note the missing
        # `std::`). 
        #
        # The current solution is to gather the tokens of the CXX_METHOD
        # cursor between the virtual keyword and the function name, and
        # pass these to `Type.from_spelling`.
        #
        # virtual const T * const f() = 0;
        #         ^^^^^^^^^^^^^^^ 
        #
        # Special care must be taken when dealing with operators.
        tokens = [t.spelling for t in cursor.get_tokens()]
        if tokens[0] == "virtual":
            tokens.pop(0)
        # Check if the name of `f` occurs in the tokens. If not, then
        # `f` is an operator.
        if f.name not in tokens:
            delim = tokens.index("operator")
        else:
            delim = tokens.index(f.name)
        f.return_type = Type.from_spelling(
            " ".join(tokens[:delim])
        )
        # The following is a hack to obtain the volatile qualifier and
        # override keywords:
        #
        # void f(...) const volatile noexcept = 0;
        #             ^^^^^^^^^^^^^^^^^^^^^^^
        #
        # Get these tokens! (There is, apparently, no other way to check
        # for cv qualifiers using python clang.)
        delim = tokens.index(")")
        keywords = tokens[delim+1:] 
        if "override" in keywords:
            f.is_override = True
        if "volatile" in keywords:
            f.is_volatile = True
        # Const qualifiers, virtual keywords, and exception
        # specifications can be obtained using python clang.
        f.is_const = cursor.is_const_method()
        f.is_pure_virtual = cursor.is_pure_virtual_method()
        if (cursor.exception_specification_kind 
            == cindex.ExceptionSpecificationKind.BASIC_NOEXCEPT
        ):
            f.is_noexcept = True
        # `f` is an operator, if its name matches the following regex.
        if f.name.replace("operator", "") in {
            "+", "-", "*", "/", "%", "^", "&", "|", "~", "!", "=", "<",
            ">", "+=", "-=", "*=", "/=", "%=", "^=", "&=", "|=", "<<",
            ">>", ">>=", "<<=", "==", "!=", "<=", ">=", "<=>", "&&",
            "||", "++", "--", ",", "->*", "->", "()", "[]"
        }:
            f.is_operator = True
        return f

    def draw(self, tab) -> str:
        s = ""
        # Templates.
        if self.template:
            s += tab*"  " + str(self.template) + "\n"
        # Virtual keyword.
        s += tab*"  " + self.is_pure_virtual*"virtual "
        # Return type.
        s += str(self.return_type) + " "
        # Function name.
        s += self.name
        # Parameters.
        s += "(" + ", ".join(str(p) for p in self.parameters) + ")"
        # Qualifiers and keywords.
        s += self.is_const*" const"
        s += self.is_volatile*" volatile"
        s += self.is_noexcept*" noexcept"
        s += self.is_override*" override"  # This must always be last!
        # Delete or print function body.
        if self.is_pure_virtual:
            s += " = 0;"
        else:
            s += "\n"
            s += self.body.draw(tab)
        return s

def get_class(
    regex: str, 
    filename: str, 
    undef: bool = True
) -> Class:
    """ Return the class declaration of `name` from the header `file`. 

    :raises ValueError: if no class declaration of `name` is found in
        `file`.
    """
    # Read the file.  
    with open(filename, "r") as file:
        source = file.read()
    # Hide macros from the preprocessor.
    if undef:
        source = undef_macros(source)
    # Parse the source and create a translation unit.
    tu = translate(filename, source)
    # Read the header file from the translation unit.
    hdr = CppFile.from_unit(tu) 
    # Find a matching class.
    cls = hdr.get_class(regex)
    if cls is None:
        raise ValueError("no class matching '{name}' found in '{file}'")
    return cls

def undef_macros(source: str) -> str:
    """ Replace all Qt macros in `source` with member declarations.

    This function preconditions undefines all of the following macros
    and replaces them with member declarations before the precompiler
    replaces the macro with its definition:

    Q_OBJECT
    
    Unlike the macro, the delcaration is now detected by the python
    clang parser. 
    """
    # Note that this approach is preferable to just searching the input
    # file for `Q_OBJECT`, as Q_OBJECT is allowed to occur in, say,
    # comments, without having any effect on the parser.
    def undef(macro: str) -> void:
        nonlocal source
        source = source.replace(macro, f"#undef {macro}\nint {macro};")
    macros = {
        "Q_OBJECT",
        # "Q_GADGET",
    }
    for macro in macros:
        undef(macro)
    return source

def translate(filename: str, source: str) -> clang.cindex.TranslationUnit:
    """ Parse `source` and create a translation unit.

    :raises RuntimeError: if the translation unit's diagnostics contains
        an error.
    """
    index = cindex.Index.create()
    Static.filename = "DRMOCK-" + filename
    tu = index.parse(
        Static.filename, 
        [
            "-x", 
            "c++", 
            "-std=" + Static.std, 
            "-fPIC",
        ] \
            + Static.compiler_flags,
        unsaved_files = [
            (Static.filename, source)
        ]
    )
    # Check for errors.
    diagnostics = list(tu.diagnostics)
    if diagnostics != []:
        error = "\n".join("\t" + str(item) for item in diagnostics)
        raise RuntimeError(
            "Translation unit failed with the following diagnostic:\n" + error
        )
    return tu

def print_cursor(cursor, depth):
    # Feel free to add further details to the print command.
    s = ""
    s += depth*"  " + str(cursor.kind)
    s += "\n"
    s += (depth + 1)*"  " + "cursor.spelling: " + str(cursor.spelling)
    s += "\n"
    s += (depth + 1)*"  " + "cursor.get_tokens(): " + str([t.spelling for t in cursor.get_tokens()])
    s += "\n"
    s += (depth + 1)*"  " + "cursor.displayname: " + str(cursor.displayname)
    s += "\n"
    print(s)

def dump(filename: str, undef: bool = True ):
    """ Parse `source` and print its ast. """
    def visit_tree(root, func, depth = 0):
        for x in Static.get_children(root):
            func(x, depth)
            visit_tree(x, func, depth + 1)
    # Read the file.  
    with open(filename, "r") as file:
        source = file.read()
    # Hide macros from the preprocessor.
    if undef:
        source = undef_macros(source)
    # Parse the source and create a translation unit.
    tu = translate(source)
    # Dump the ast.
    visit_tree(tu.cursor, print_cursor)


def configure(
    std: str, 
    includes: List[str] = [], 
    frameworks: List[str] = [],
    before_install: bool = False
):
    """ Setup python clang for usage. 

    The paths in `"mocker.cfg"` are expected to be native UNIX paths.
    """
    def without_escapes(path: str):
        return str.replace(path, "\\ ", " ").lstrip()
    # Read the mocker config file.
    config_file = resource_filename(__name__, "mocker.cfg")
    config = configparser.ConfigParser()
    config.read(config_file)

    # Get the clang path.
    library_path = config["userData"]["clangPath"]
    cindex.Config.set_library_file(library_path)

    # Set additional include paths.
    # If on macos, then include the qt framework path and the Xcode root.
    if sys.platform == "darwin":
        Static.compiler_flags.extend(
            ["-iframework" + without_escapes(path) for path in frameworks]
        )
        # Get the Xcode SDK path.
        tmp = subprocess.check_output(["xcrun", "--show-sdk-path"])
        sdk = tmp.decode(sys.stdout.encoding).rstrip("\n")
        # Add the compiler flag.
        Static.compiler_flags.extend(["-isysroot" + sdk])

    # Get the qt5 include paths.
    Static.compiler_flags.extend(
        ["-I" + without_escapes(path) for path in includes]
    )

    # Get the C++ standard.
    Static.std = std

    # Set the correct include prefix for headers from libdrmock
    if before_install:
        Static.prefix = ""
    else:
        Static.prefix = "DrMock/"
