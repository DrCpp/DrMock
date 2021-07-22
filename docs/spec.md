<!--
Copyright 2021 Ole Kliemann, Malte Kliemann

This file is part of DrMock.

DrMock is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

DrMock is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with DrMock.  If not, see <https://www.gnu.org/licenses/>.
-->

# DrMock specification

## Table of contents

- [Informal Introduction]
- [Terminology]
  + [Keywords]
- [Interface]
- [Mock Implementation]
- [Mock Object]
- [Matching]


## Informal Introduction

The **DrMock** specification defines a process by which, for an input
C++ class (the _interface_), the code of a child class, the _mock
implementation_ is returned, which may be configured in certain fashion
to mimic the expected behavior of the interface.
This process is implemented by
[drmock-generator](https://github.com/DrCpp/drmock-generator), which
uses the code from the `drmock::` namespace of **DrMock**. The CMake
macros of **DrMock** invoke `drmock-generator` for ease of use. The
process is called _mocking_ or _generating a mock_.

(It is useful to think of `drmock` and `drtest` as two seperate
projects. The code of `drmock` is consumed by the `drmock-generator`;
`drtest`, on the other hand, is a seperate unit-test framework. The
CMake macros should probably be viewed as belonging to `drtest`.
However, `drmock` and `drtest` are currently still part of the same C++
library.)


## Terminology

### Keywords

The bold-face terms **must**, **must not**, **should**, **should not**
and **may** used below are defined in [Key words for use in RFCs to
Indicate Requirement Levels](https://www.ietf.org/rfc/rfc2119.txt).


## User Input

The following input data is specified by the user:

- A relative or absolute filesystem path to a file which contains the
  input class
- A relative or absolute filesystem path to the output .h file
- The name of the interface class
- A set of _target access specifiers_ (only methods with these access
  specifiers will be mocked)
- A name for the mock implementation (see
  [Mock Implementation] for details)
- A global or relative _enclosing namespace for mocking_
- A name for the _controller_ member variable of the mock object


## Interface

The _input class_ or _interface_ is subject to a few conditions that it
**must** satisfy in order to be _mockable_. A non-mockable input class
will result in undefined behavior.

Let `file.h` be a C++ header file which contains a class declaration
`Interface` ("the declaration").
Then `Interface` is mockable if:

- The interface's name **must not** contain the substring `DRMOCK`
- `Interface` **must not** be declared `final`
- The declaration **must not** contain conversion operators
- All type references occuring in the declaration **must** be declared
  with their full enclosing namespace
- If the declaration contains an operator with symbol `SYMBOL`,
  then `Interface::` **must not** contain a method called
  `operator{designator(SYMBOL)}`, where `designator` is defined
  according to the table at the end of this section
- `Interface::` **must not** have a member called `mock`
- `Interface::` **must not** have a member whose name contains the
  substring `DRMOCK`
- The declaration **must not** have virtual volatile-qualified methods
- The interface **must not** be contained in the global namespace
- The enclosing namespace for mocking **must not** contain a member with
  the name of the mock object or the name of the mock implementation
- `Interface::` **must not** have a virtual method whose name is equal
  to the name of the controller of the mock object (see [User Data])

Note the distinction between `Interface` and _the declaration_. For
example, a parent of `Interface` may implement a conversion operator.

For the sake of clarity, we list some of the possible qualities of
mockable class:

- `Interface` **may** be abstract or non-abstract
- `Interface` **may** be a class template (but the template parameters
  cannot contain the substring `DRMOCK`)
- The declaration **may** contain type alias (template) declarations
- The declaration **may** contain static methods
- The declaration **may** contain const-qualified and ref-qualified
  methods
- The declaration **may** contain `noexcept` methods
- The declaration **may** contain non-virtual volatile-qualified methods
- Parents of `Interface` **may** contain virtual volatile-qualified methods
- Parents of `Interface` **may** contain conversion operators
- The declarations of parents of `Interface` **may** contain type
  references that are not declared with their full enclosing namespace

Furthermore, the following assumptions are made:

- The namespace `drmock::` is reserved for the C++ implementation of the
  **DrMock** specification and **must not** contain any user code

A method _f_ (each overload is counted as seperate method) is _mockable_ if
it satisfies the following properties:

- _f_ is delcared `virtual` (in particular, _f_ is not delcared `static`)
- _f_ is not declared `volatile`
- _f_ is not a conversion operator

The rules above say, among other things, that every pure virtual method
must be mockable.

| Symbol | Designator     | Symbol     | Designator          |
| :----- | :------------- | :--------- | :------------------ |
| `+`    | `Plus`         | `\|=`      | `PipeAssign`        |
| `-`    | `Minus`        | `<<`       | `StreamLeft`        |
| `*`    | `Ast`          | `>>`       | `StreamRight`       |
| `/`    | `Div`          | `>>=`      | `StreamRightAssign` |
| `%`    | `Modulo`       | `<<=`      | `StreamLeftAssign`  |
| `^`    | `Caret`        | `==`       | `Equal`             |
| `&`    | `Amp`          | `!=`       | `NotEqual`          |
| `\|`   | `Pipe`         | `<=`       | `LesserOrEqual`     |
| `~`    | `Tilde`        | `>=`       | `GreaterOrEqual`    |
| `!`    | `Not`          | `<=>`      | `SpaceShip`         |
| `=`    | `Assign`       | `&&`       | `And`               |
| `<`    | `Lesser`       | `\|\|`     | `Or`                |
| `>`    | `Greater`      | `++`       | `Increment`         |
| `+=`   | `PlusAssign`   | `--`       | `Decrement`         |
| `-=`   | `MinusAssign`  | `,`        | `Comma`             |
| `*=`   | `AstAssign`    | `->*`      | `PointerToMember`   |
| `/=`   | `DivAssig`     | `->`       | `Arrow`             |
| `%=`   | `ModuloAssign` | `()`       | `Call`              |
| `^=`   | `CaretAssign`  | `[]`       | `Brackets`          |
| `&=`   | `AmpAssign`    | `co_await` | `CoAwait`           |



## Mock Object

Let `NS0::...::NSn::Interface` ("the interface") be a
mockable interface. The _mock object_ of this interface is a C++ class
with the following properties:

- The mock object's name is as specified by the user
- The enclosing namespace of the mock object **must be**
  the enclosing namespace for mocking
- The mock object has a member of type `::drmock::Controller` ("the
  controller") whose name is specified by the user (`control` for
  purposes of demonstration)
- The mock object is declared a `friend` of the mock implementation of
  the interface
- The mock object has a member of type
  `std::shared_ptr<::drmock::StateObject>` ("the state object")

Furthermore, the mock object **must** define one member variable of type
`std::shared_ptr<::drmock::Method<Instance, T, Ts...>>` for every
mockable method `T Instance::func(Ts...)`, one for each overload. Each
must be initialized with the name of the associated function and the state object,
e.g. `{"func", DRMOCK_STATE_OBJECT_}`.

These smart pointers are called the smart pointers _associated with_ `T
func(Ts...)`. (The `Method` object simulates the behavior of the
implementation of `func` in the mock implementation.) The controller
**must** be initialized with an instance of
`std::vector<std::shared_ptr<IMethod>>` which contains all smart
pointers.

Finally, the mock object **must** define a set of methods called
_getters_:

- For every _non-overloaded_ mockable method, say
  `T Interface::func(Ts...) [...]`,
  the mock object **must** define a public method `auto& func()` (no
  qualifiers) which returns a reference to the smart pointer associated
  with `T func(Ts...)`.

- For every set of overloaded mockable methods (collectively, "the set"), say `Interface::func`,
  the mock object **must** declare a public method
  `template<typename... Us> auto& func()` and, for every overload of the
  set, define a specialization which returns the smart pointer
  associated with said overload;
  the template parameters
  are used to specify the parameters and cv/reference-qualifiers of the
  requested overload.
  Their behavior is defined as follows, depending on the template
  arguments:

  + If all members of the set have the same parameters `Ts...` (but different
    const/reference qualifiers), then...
    - `MockObject::func<>()` returns the smart pointer associated with
      `T func(Ts...)`
    - `MockObject::func<::drmock::LValueRef>` returns the smart pointer
      associated with `T func(Ts...)&`
    - `MockObject::func<::drmock::RValueRef>` returns the smart pointer
      associated with `T func(Ts...)&&`
    - `MockObject::func<::drmock::Const>()` returns the smart pointer
      associated with `T func(Ts...) const`
    - `MockObject::func<::drmock::Const, ::drmock::LValueRef>()`
      returns the smart pointer associated with `T func(Ts...) const&`
    - `MockObject::func<::drmock::Const, ::drmock::RValueRef>()`
      returns the smart pointer associated with `T func(Ts...) const&&`

    (Note that `T` denotes potentially varying return types between these
    overloads. Mind the correct order of `::drmock::Const` and
    `::drmock*ValueRef`!)

  + If all members of the set have the same cv/reference-qualifiers
    `[...]`, then `MockObject::func<Ts...>()` return the smart pointer
    associated with `T func(Ts...)`.

    (This means that `::drmock::Const` need not be specified if all
    overloads are const, for example.)

  + If neither parameters or cv/reference-qualifiers match for all
    members of the set, then...
    - `MockObject::func<Ts...>()` returns the smart pointer associated with
      `T func(Ts...)`
    - `MockObject::func<Ts..., ::drmock::LValueRef>` returns the smart pointer
      associated with `T func(Ts...)&`
    - `MockObject::func<Ts..., ::drmock::RValueRef>` returns the smart pointer
      associated with `T func(Ts...)&&`
    - `MockObject::func<Ts..., ::drmock::Const>()` returns the smart pointer
      associated with `T func(Ts...) const`
    - `MockObject::func<Ts..., ::drmock::Const, ::drmock::LValueRef>()`
      returns the smart pointer associated with `T func(Ts...) const&`
    - `MockObject::func<Ts..., ::drmock::Const, ::drmock::RValueRef>()`
      returns the smart pointer associated with `T func(Ts...) const&&`

    (Note that `T` denotes potentially varying return types between these
    overloads. Mind the correct order of `::drmock::Const` and
    `::drmock*ValueRef`!)


## Mock Implementation

The _mock implementation_ is the output of the mocking process. Let
`NS0::...::NSn` be the  enclosing namespace for mocking (see [User Input]).
Then the mock implementation is a class with following properties:

- The name of the mock implementation is chosen by the user (for
  demonstration purposes, we call the class `MockImplementation`)
- The enclosing namespace of the mock implementation **must be**
  the enclosing namespace for mocking
- The mock implementation **must** be a public subclass of the interface
- The mock implementation **must** implement a forwarding constructor
  which forwards its arguments to the constructor of the interface and
  calls each of mock object's `std::shared_ptr<IMethod>` objects
  `parent` method with `this`
- The mock implementation **must** contain a mutable public member
  `mock` of type _(the mock object of the interface)_ (see [The Mock
  Object])

Furthermore, the mock implementation **must** implement every mockable
method `T func(Ts...) [qualifiers]`
from the interface

```
T f(Ts... ts) [qualifiers] override {
  mock.template f<Ts...>().call((ts)...);
}
```

- references are passed by reference to the `call` method
- lvalues are moved to the `call` method


## Matching


These values may either be `std::shared_ptr<IMatcher<Args>>...` or
_naked_ `Args...`.
If a naked argument `arg` is passed, it is automatically placed in a matcher
which matches _equality_, and **must** therefore satisfy the following
properties:

1. `Arg` is not abstract and implements `bool operator==(const Arg&) const`
2. `Arg` is the same as `std::shared_ptr<T>` or `std::unique_ptr<T>`
  where `T` is a type that satisfies 1., 2., or 3. or is abstract
3. `Arg` is the same as `std::tuple<Ts...>` where `Ts...` satisfy 1., 2. or 3.

**must** be contained 

## Example

For example:

```cpp
class Interface {
public:
  static void static_func(int, float);

};
```

- Show example mock object and implementation

Note that, for example, the _dispatch_ methods of the mock object are an
implementation detail

[Informal Introduction]: #informal-introduction
[Keywords]: #keywords
[Interface]: #interface
[Mock Object]: #mock-object
[Mock Implementation]: #mock-implementation
[Terminology]: #terminology
[Matching]: #matching
