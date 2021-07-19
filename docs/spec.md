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

- [Informal Introduction][]
- [Terminology][]
- + [Keywords][]
- [The Interface][]
- [The Mock Implementation][]
- [The Mock Object][]
- [Matching][]


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

other stuff from drmock-generator
mock namespace; let's say it's `ns`



## The Interface

The _input class_ or _interface_ is subject to a few conditions that it
**must** satisfy in order to be _mockable_. A non-mockable input class
will result in undefined behavior.

Let `file.h` be a C++ header file which contains a class declaration
`Interface` (_the declaration_). Let `Ns0::...::NSn` be the enclosing
namespace of the interface. Then `Interface` is mockable if:

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
- The declaration **must not** have volatile-qualified methods
- The interface **must not** be contained in the global namespace
- `NSn` does not contain a namespace `NSn::drmock`

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
- Parents of `Interface` **may** contain volatile-qualified methods
- Parents of `Interface` **may** contain conversion operators
- The declarations of parents of `Interface` **may** contain type
  references that are not declared with their full enclosing namespace

Furthermore, the following assumptions are made:

- The namespace `drmock::` is reserved for the C++ implementation of the
  **DrMock** specification and **must not** contain any user code

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



## The Mock Object

Let 


- The name of the mock object **must** start with the prefix `DRMOCK_`;
  the generator must create a unique
- Declares the mock implementation


## The Mock Implementation

The _mock implementation_ is the output of the mocking process. Let
`NS0::...::NSn::Interface` be as in [The Interface][] and `NS0::...::NSn::ns::DRMOCK_MockObject`.
Then the mock implementation is a class with following properties:

- The enclosing namespace of the mock implementation **must be**
  `NS0::...::NSn::ns`
- The name of the mock implementation is chosen by the user (for
  demonstration purposes, we call the class `MockImplementation`)
- The mock implementation is a public subclass of the interface
- The mock implementation contains a mutable public member `mock` of
  type _(the mock object of the interface)_ (see [The Mock Object])


## Matching


These values may either be `std::shared_ptr<IMatcher<Args>>...` or
_naked_ `Args...`.
If a naked argument `arg` is passed, it is automatically placed in a matcher
which matches _equality_, and **must** therefore satisfy the following
properties:

1. `Arg` is not abstract
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
