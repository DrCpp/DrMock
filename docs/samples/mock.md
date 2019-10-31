<!-- 
Copyright 2019 Ole Kliemann, Malte Kliemann

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

# samples/mock

This sample demonstrates the basics of **DrMock**'s mock features.

### Table of contents

* [Introduction](#introduction)
* [Setup](#setup)
* [Running the tests](#running-the-tests)
* [Behaviors](#behaviors)
  + [times](#times)
  + [persists](#persists)
  + [expects](#expects)
  + [returns](#returns)
  + [throws](#throws)
* [Details](#details)
  + [Interface methods](#interface-methods)
  + [Polymorphism](#polymorphism)
  + [Operators](#operators)
  + [Changing nomenclature templates](#changing-nomenclature-templates)
  + [Ignore order of behaviors](#ignore-order-of-behaviors)
  + [DrMockModule documentation](#drmockmodule-documentation)
* [Caveats](#caveats)
  + [Raw pointers as parameters](#raw-pointers-as-parameters)
* [Fine print: Interface](#fine-print-interface)
* [Bibliography](#bibliography)

### Project structure

```
samples/mock
│   CMakeLists.txt
│   Makefile
│
└───src
│   │   CMakeLists.txt
│   │   IWarehouse.h
│   │   Order.cpp
│   │   Order.h
│   │   Warehouse.cpp
│   │   Warehouse.h
│   
└───tests
    │   CMakeLists.txt
    │   OrderTest.cpp
```

Note that this samples uses the typical CMake project structure with
three `CMakeLists.txt`. `src/CMakeLists.txt` manages the source files
and `tests/CMakeLists.txt` the tests.

### Requirements

This project requires an installation of **DrMock** in `prefix/` or the
`CMAKE_PREFIX_PATH`. If your installation of **DrMock** is located
elsewhere, you must change the `-DCMAKE_PREFIX_PATH=...` flag in
`Makefile`.

## Introduction

**DrMock** generates source code of mock objects from _interfaces_. The
definition of the notion of _interface_ is given [below](#interface).
Let's introduce the concept with an example from M. Fowler's _Mocks
Aren't Stubs_ [1].

At an online shop, customers fill out an `Order` with the commodity
they'd like to order and the order quantity. The `Order` may be filled
from a `Warehouse` using `fill`. But `fill` will fail if the `Warehouse`
doesn't hold the commodity in at least the quantity requrested by the
customer. We want to test this interaction between `Order` and
`Warehouse`, and, assuming that the unit `Warehouse` has already been
tested, will use a mock of `Warehouse` for this purpose.

That means that we must specify an interface `IWarehouse` that
`Warehouse` will then implement:
```cpp
// IWarehouse.h

// ...

class IWarehouse
{
public:
  virtual ~IWarehouse() = default;

  virtual void add(std::string commodity, std::size_t quantity) = 0;
  virtual bool remove(const std::string& commodity, std::size_t quantity) = 0;
};
```

The basic requirement of **DrMock** is that the interface is _abstract_
(contains only pure virtual methods). Obviously, this is satisfied. In
particular, interfaces are polymorphic.

The example interface declares two pure virtual methods `add` and
`remove`, which the _implementation_ must then define.  Note that
`remove` returns a `bool`. If removing fail due to too large a quantity
being requested, this should be `false`. Otherwise, `true`.  Although it
is irrelevant here, the reader is invited to look at the sample
implementation found in `Warehouse.h` and `Warehouse.cpp`.

Let's take a short peek at the header of `Order` for a moment. 
```cpp
// Order.h

// ...

class Order
{
public:
  Order(std::string commodity, std::size_t quantity);

  void fill(const std::shared_ptr<IWarehouse>&);
  bool filled() const;

private:
  bool filled_;
  std::string commodity_;
  std::size_t quantity_;
};
```
Note that the parameter of `fill` is an `std::shared_ptr` to allow the
use of polymorphism. You could use virtually any type of (smart) pointer
in place of `std::shared_ptr`. 

Here's the straightforward implementation of the `fill` method:
```cpp
void 
Order::fill(const std::shared_ptr<IWarehouse& wh)
{
  filled_ = wh->remove(commodity_, quantity_);
}
```
Thus, the order will attempt to remove the requested amount of units of
the commodity from the warehouse and set `filled_` accordingly.

## Setup

To use **DrMock** to create source code for a mock of `IWarehouse`, the
macro `DrMockModule` is used. 
```cmake
# src/CMakeLists.txt

add_library(DrMockSampleMock SHARED
  Order.cpp
  Warehouse.cpp
)

file(TO_CMAKE_PATH
  ${CMAKE_SOURCE_DIR}/../../python/DrMockGenerator
  pathToMocker
)
DrMockModule(
  TARGET DrMockSampleMockMocked
  GENERATOR ${pathToMocker}
  HEADERS
    IWarehouse.h
)
```
Under `HEADER`, **DrMock** expects the headers of the interfaces that
must be mocked. For each argument under `HEADER`, **DrMock** will
generate the source code of respective mock object and compile them into
a library, `DrMockSampleMockMocked` (if you think that's silly, then by
all means, change the libraries name using the `TARGET` parameter).

What's the `GENERATOR` parameter for? Per default, `DrMockModule`
expects the `DrMockGenerator` python script to be installed somewhere in
`PATH`. If that is not the case, the path to the script must be
specified by the user. If you've already installed `DrMockGenerator`,
try removing the `GENERATOR` argument.

A detailed documentation may be found in [DrMockModule documentation](#drmockmodule-documentation).

Let's now see how mock objects are used in tests. First, take a look at
`tests/CMakeLists.txt`. 
```cmake
DrMockTest(
  LIBS
    DrMockSampleMock
    DrMockSampleMockMocked
  TESTS
    OrderTest.cpp
)
```
The call of `DrMockTest` has changed as we've added the parameter
`LIBS`. This parameter tells **DrMock** which libraries to link the
tests (i.e. the executables compiled from `TESTS`) against. In this
case, the test `OrderTest.cpp` requires the class `Order` from
`DrMockMockingSample` and, of course, the mock of `IWarehouse` from
`DrMockMockingSampleMocked`.

## Using the mock object

So, what's going on in `OrderTest.cpp`? First, note the includes:
```cpp
#include "mock/WarehouseMock.h"
```
For every file under `HEADER`, say `path/to/IFoo.h`, **DrMock**
generates header and source files `FooMock.h` and `FooMock.cpp`, which
may be included using the path `mock/path/to/IFoo.h`. In these files,
the mock class `FooMock` is defined. You must strictly follow this
template: The class and filename of every interface must begin with an
`I`, and the mock object will be named accordingly.  (You can change
these nomenclature templates, but more of that later) The path is
_relative to the current CMake source dir_. Thus, calling `DrMockModule`
from anywhere but `src/CMakeLists.txt` is bound to result in odd include
paths.

Let's go through the first test. The following call makes a shared
`WarehouseMock` object and sets some of its properties.  Note that the
mock of `IWarehouse` is placed in the same namespace as its interface.
```cpp
auto warehouse = std::make_shared<drmock::samples::WarehouseMock>();
warehouse->mock.remove().push()
    .expects("foo", 2)
    .times(1)
    .return(true);
```
So, what's this? Every mock, contains a public instance of type
`DRMOCK_Object_Warehouse` (or whatever the implementation's name is),
whose source code is generated alongside that of `WarehouseMock`. This
_mock object_ lets the user control the expected behavior of the mock
obejct.

For instance, to define the behavior of `remove`, you call
`warehouse->mock.remove()`. This returns a `Method` object onto which
`Behavior`s may be pushed using `push` (detail below). Here, the mock
object is instructed to expect the call `remove("foo", 2)` _exactly
once_ (call `times` with parameter 1), and then to return `true`. 

**Note.** The `push` method, as well as `expects`, `times`, etc. returns
a reference to the pushed behavior, thus allowing the user to
concatenate the calls as above.

Now that the behavior of `warehouse` is defined, the order for two units
of foo is filled from the warehouse. Judging from the implementation of
`fill`, this should call `warehouse->remove("foo", 2)`. And, ss defined
earlier, removing two units of foo will succeed. Whether the defined
behavior occured of not may be verified using the following call:
```cpp
DRTEST_ASSERT(warehouse->mock.verify());
```
Or, if you prefer:
```cpp
DRTEST_VERIFY_MOCK(warehouse->mock);
```
After verifying the mock, we check that the `filled` method returns the
correct value:
```cpp
DRTEST_ASSERT_EQ(order.filled(), true);
```

**Note.** When verifying the mock object, the `Behavior`s are expected
to occur in the order in which they were pushed. See also: [Ignore order
of behavior](#ignore-order-of-behavior).

The second test runs along the same lines. Once again, the customer
places an order for two units of foo, but this time the call will fail:
```cpp
auto warehouse = std::make_shared<drmock::samples::WarehouseMock>();
warehouse->mock.remove().push()
    .expects("foo", 2)
    .times(1)
    .return(false);
```
Once again, `warehouse->mock` is verified, and `order.filled()` is now
expected to return `false`.

## Running the tests

Do `make` to run the tests. The following should occur:
```
    Start 1: OrderTest
1/1 Test #1: OrderTest ........................   Passed    0.00 sec

100% tests passed, 0 tests failed out of 1

Total Test time (real) =   0.01 sec
```

## Behaviors

Recall that a call like `warehouse->mock.remove()` returns a `Method`
object that owns a queue (called the `BehaviorStack`) onto which
`Behavior`s may be pushed using `Method::push`. 

Everytime the underlying `Method` is called, the 
This section briefly describes how `Behavior`
instances may be configured.

### times

Every `Behavior` _persists_ for a number of calls made to the underlying
`Method` before it expires. Using `times(unsigned int)`, this number of
calls is set. The default value is `1`.

See also: [persists](#persists).

### persists

Every `Behavior` _persists_ for a number of calls made to the underlying
`Method` before it expires. Calling `persists()` makes the `Behavior`
immortal - it will never expire, no matter how many calls are made.

See also: [times](#times).

### expects

Use `expects(Args... args)` to instruct the `Behavior` to expect the
underlying method to be called with `args...` as arguments. 

### returns

Use `returns(T&& result)` to instruct the `Behavior` to return `result`
on every method call that triggers it. Here `T` is the underlying
`Method`'s return value type.

### throws

Use `throws(E&& exception)` to instruct the `Behavior` to throw
`exception` on every method call that triggers it.

## Details

### Interface methods

When declaring a method in an interface, all type references occuring in
the declaration of the parameters and return values must be declared
with their full enclosing namespace.

In other words, this is wrong (although it will compile):
```cpp
namespace drmock { namespace samples {

class Foo {};
class Bar {};

class IBaz
{
public:
  virtual ~IBaz() = default;

  Bar func(Foo) const = 0;
};

}} // namespace drmock::samples
```

Instead, the declaration of `func` must be:
```cpp
drmock::samples::Bar func(drmock::samples::Foo) const = 0;
```

**DrMock** also requires that parameters of interface method be
_comparable_. Thus, they must implement `operator==`, with the following
exceptions: 

(1) `std::shared_ptr<T>` and `std::unique_ptr<T>` are comparable if `T`
is comparable. They are compared by comparing their pointees.  If the
pointee type `T` is abstract, polymorphism must be specified, see below.

(2) `std::tuple<Ts...>` is comparable if all
elements of `Ts...` are comparable. 

### Polymorphism

If an interface's method accepts an `std::shared_ptr<B>` or
`std::unique_ptr<B>` with abstract pointee type `B`, then the `Method`
object must be informed informed which derived type to expect using the
`polymorphic` method.

For example,
```cpp
class Base 
{
  // ...
};

class Derived : public Base 
{
  // Make `Derived` comparable.
  bool operator==(const Derived&) const { /* ... */ }

  // ...
};

// IFoo.h

class IFoo 
{
public:
  ~IFoo() = default;

  void func(std::shared_ptr<Base>, std::shared_ptr<Base>) = 0;
};
```

Then, use `polymorphic` to register `Derived`:
```cpp
auto foo = std::make_shared<FooMock>();
foo->mock.func().polymorphic<std::shared_ptr<Derived>, std::shared_ptr<Derived>>();
foo->mock.func().expects(
    std::make_shared<Derived>(/* ... */), 
    std::make_shared<Derived>(/* ... */)
  );
```

### Operators

Mocking an operator declared in an interface is not much different from
mocking any other method, only the way in which the mocked method is
accessed from the mock object changes. Instead of doing
```cpp
foo->mock.operator*().expects(/* ... */);
```
(which is illegal), you must do
```cpp
foo->mock.operatorAst().expects(/* ... */);
```
What's this? The illegal tokens are replaced with a designator
describing the operators symbol. The designators for C++'s overloadable
operators are found in the table below.

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

Thus, `operator+` gets the handle `operatorPlus`, `operator%=` gets
`operatorModuloAssign`, etc.

### Changing nomenclature templates

**DrMock** uses the following template for identifying interfaces and
their mocks: Interfaces begin with `I`, followed by the class name
prefered for the implementation, such as `IVector` for the class
`Vector`. The associated mock object is designated by the implementation
name followed by `Mock`. For example, `VectorMock`. The same template is
applied to the header and source filenames.

If you don't wish to follow this template, you must change the call to
`DrMockModule`. The arguments of `IFILE` and `ICLASS` must be regular
expression with exactly one capture group, those of `MOCKFILE` and
`MOCKCLASS` must contain a single subexpression `\1` (beware the CMake
excape rules!). 

The capture groups gather the implementation file and class name and
replace said subexpressions to compute the mock file and class name. For
example, the following configures **DrMock** to expect interfaces of the
form `interface_vector`, etc. and to return mock objects called
`vector_mock`, etc.:
```cmake
DrMockModule(
  TARGET MyLibMocked
  IFILE
    "interface_([a-zA-Z0-9].*)"
  MOCKFILE
    "\\1_mock"
  ICLASS
    "interface_([a-zA-Z0-9].*)"
  MOCKCLASS
    "\\1_mock"
  HEADERS
    interface_vector.h
    interface_matrix.h
    # ...
)
```

### Ignore order of behaviors

Recall that `Behavior`s are expected to occur in the order in which they
are pushed onto the `Method`'s `BehaviorStack`. This can be disabled by
calling `Method::enfore_order` as follows:
```cpp
warehouse->mock.remove().enforce_order(false);
```
If `enforce_order` is disabled, and the mocked method is called, the
first `Behavior` on the `BehaviorStack` that matches the method call is
triggered.

### DrMockModule documentation
```cmake
DrMockModule(
  TARGET 
  HEADERS header1 [header2 [header3 ...]]
  [IFILE]
  [MOCKFILE]
  [ICLASS]
  [MOCKCLASS]
  [GENERATOR]
  [LIBS lib1 [lib2 [lib3 ...]]]
  [QT]
  [INCLUDE include1 [include2 [include3 ...]]]
  [FRAMEWORKS framework1 [framework2 [framework3 ...]]]
)
```

##### TARGET
  The name of the library that is created.

##### HEADERS
  A list of header files. Every header file must match the regex
  provided via the `IFILE` argument.

##### IFILE
  A regex that describes the pattern that matches the project's
  interface header filenames. The regex must contain exactly one
  capture group that captures the unadorned filename. The default
  value is ``I([a-zA-Z0-9].*)"`.

##### MOCKFILE
  A string that describes the pattern that the project's mock object
  header filenames match. The string must contain exactly one
  subexpression character `"\\1"`. The default value is `"\\1Mock"`.

##### ICLASS
  A regex that describes the pattern that matches the project's
  interface class names. The regex must contain exactly one capture
  group that captures the unadorned class name. Each of the specified
  header files must contain exactly one class that matches this regex.
  The default value is `IFILE`.

##### MOCKCLASS
  A string that describes the pattern that the project's mock object
  class names match. The regex must contain exactly one subexpression
  character `"\\1"`. The default value is `MOCKFILE`.

##### GENERATOR 
  A path to the generator script of DrMock. Default value is the
  current path.

##### LIBS
  A list of libraries that `TARGET` is linked against. Default value
  is an empty list.

##### QT
  If `QT` is set, the `HEADERS` will be added to the sources of
  `TARGET`, thus allowing the interfaces that are Q_OBJECT to be
  mocked. Default value is `OFF`.
  
##### INCLUDE
  A list of include path's that are required to parse the `HEADERS`.
  Default value is an empty list.

##### FRAMEWORKS
  A list of macOS framework path's that are required to parse the
  `HEADERS`. Default value is an empty list.

## Caveats

### Raw pointers as parameters

While technically not prohibited, the use of raw pointers as parameters
or return values of interface methods will most likely lead to undesired
results: Raw pointers are comparable (cf. [Interface methods](#interface-methods)),
but they are compared directly. Thus, the following will fail:
```cpp
int* x = new int{0};
int* y = new int{0};  // *x == *y
object->mock.func().expects(x);
object->func(y);  // x != y
DRTEST_ASSERT(object->mock.verify());  // Expected `x`, but received `y`.
```

## Fine print: Interface

Here's the definition of the notion of _interface_ in the context of a
call of `DrMockModule`:

* The file's name with its file extension<sup>1</sup> removed shall
  match `IFILE`.

* The file shall contain exactly one class declaration whose name
  matches `ICLASS`.

<sup>1</sup>: By *file extension* we mean the substring spanning from the first
  `.` until the string's end (i.e., the substring that matches
  `\.[^.]*$`).  For instance, the file extension of `file.tar.gz` is
  `.tar.gz`.

The *interface* is the unique class discovered determined by `ICLASS` as
described above and must satisfy the following conditions:

* The only declarations in the interface shall be public methods and
  type alias (template) declarations.<sup>2</sup>

* All methods shall be declared pure virtual.<sup>2</sup>

* The interface shall only derive from non-abstract classes.

* The interface shall not contain any conversion functions.

* If an operator is defined in the interface, the interface shall not
  have a method called `operator[SYMBOL]`, where `[SYMBOL]` is
  determined by the operator's symbol according to the table in
  [Operators](#operators).

* None of the interface's method shall be a volatile qualified method
  or a method with volatile qualified parameters.

* All type references occuring in the declarations of the parameters and
  return values of interface's methods shall be declared with their full
  enclosing namespace.

* Every parameter or return value `Foo` of the interface's methods shall
  satisfy one of the following conditions:

  (1) `Foo` is not abstract and implements 
    `bool operator==(const Foo&) const`.

  (2) It is an `std::shared_ptr` or an `std::unique_ptr` to a type that
    satisfies (1), (2) or (3) or is an abstract class.

  (3) It is an `std::tuple` of types that satisfy (1), (2) or (3).

<sup>2</sup>: QObjects are exceptions to these rule, see [samples/qt](samples/qt)
  below.

## Bibliography

[1] [M. Fowler, _Mocks Aren't Stubs_](https://martinfowler.com/articles/mocksArentStubs.html)
