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

# Using DrMock

Sample projects that use DrMock for testing may be found in `samples/`.
This tutorial will take you through all of them step-by-step.

* [samples/basic](#samplesbasic) 
  Learn **DrMock**'s basic testing features.

* [samples/mock](#samplesmock) 
  Learn how to create and use **DrMock**'s mock objects.

* [samples/qt](#samplesqt) 
  Learn how to mock `Qt5/QObject`s.

## samples/basic

This sample demonstrates the basic testing capabilities of **DrMock**.

```
samples/basic
│   CMakeLists.txt
│   Makefile
│   basicTest.cpp
```

### Setup

Recall that **DrMock** requires you to configure your project with
CMake. The `CMakeLists.txt` is faily simple. To fullfil **DrMock**'s
requirement for C++17, do

```cmake
set(CMAKE_CXX_STANDARD 17)
```

Next, find the package `DrMock::Core`.

```cmake
find_package(DrMock COMPONENTS Core REQUIRED)
```

This will fail if `DrMock::Core` cannot be found in the
`CMAKE_PREFIX_PATH`. Make sure that DrMock installed there, or set the
prefix in the `Makefile` (see below). 

Finding the **DrMock** package
automatically import the macro `DrMockTest`, which may be used to
register tests.

```cmake
enable_testing()
DrMockTest(TESTS
  basicTest.cpp
)
```

You could add further tests by providing addition arguments to the
macro, like so:

```cmake
DrMockTest(TESTS
  test0.cpp
  test1.cpp
  # ...
)
```

Now lets take a look at the `Makefile`:

```makefile
default:
  mkdir -p build && cd build && cmake .. -DCMAKE_PREFIX_PATH=../../prefix && make -j10 && ctest --output-on-failure

clean:
  rm -fr build
```

If you haven't already, you can set the `CMAKE_PREFIX_PATH` to the
installation directory of the **DrMock** package.  If you've done `make
&& make install` in the **DrMock** source folder and haven't moved
`prefix`, the default `CMAKE_PREFIX_PATH` set in the `Makefile` is
referencing the correct directory. 

### Source code

Time to look at some test code. Open `basicTest.cpp`! 

`DrMock/Test.h` contains **DrMock**'s test macros and must be included
in every test source file. To open a new test, do
```cpp
DRTEST_TEST(someTest)
{
  // ...
}
```
A source file name hold an arbitrary number of `DRTEST_TEST(...)` calls.

#### Assertion and comparison

Use `DRTEST_ASSERT` to check if an expression is `true`:
```cpp
DRTEST_TEST(someTest)
{
  DRTEST_ASSERT(true);
  DRTEST_ASSERT(3 + 4 == 7);
}
```

To check equality of objects of some type `T`, the `DRTEST_COMPARE`
macro may be used if `T` implements
`std::ostream& operator<<(ostream& os, const T&)`,
as done in the next test.
```cpp
DRTEST_TEST(anotherTest)
{
  DRTEST_COMPARE(3 + 4, 7);
}
```
The `DRTEST_COMPARE` macro will print the left- and right-hand side of
the comparison in case of failure.
If the compare type `T` doesn't provide a streaming operator, use
`DR_TEST(lhs == rhs)` instead.

#### Catching exceptions

To check for exceptions, use 
```cpp
DRTEST_ASSERT_THROW(statement, exceptionType);
```
which executes `statement` and checks if an exception of type
`exceptionType` is thrown:

```cpp
DRTEST_TEST(exceptionTest)
{
  DRTEST_ASSERT_THROW(
      throw std::runtime_error{"foo"},
      std::runtime_error
    );

  // ...
}
```
If you wish, you can check multiple statements at once:
```cpp
DRTEST_ASSERT_THROW(
    std::string str = "foo";
    throw std::runtime_error{str};
    (void)str,  // Mark `str` as used.
    std::runtime_error
  );
```
Observe that the statements are seperated by semicolons.

To check for any throw, use `DRTEST_ASSERT_FAIL`:
```cpp
DRTEST_ASSERT_FAIL(throw std::runtime_error{"foo!"});
```

#### Test tables

Test data is organised in _test tables_.
To initialize a test table for the test `someTestWithTable`, use
```cpp
DRTEST_DATA(someTestWithTable)
{
  // ...
}
```
Columns and rows can be pushed to the table by using
```cpp
drtest::addColumn<Type>("column_name");
```
For instance,
```cpp
DRTEST_DATA(someTestWithTable)
{
  drtest::addColumn<int>("lhs");
  drtest::addColumn<int>("rhs");
  drtest::addColumn<int>("expected");
  drtest::addColumn<std::string>("randomStuff");

  // ...
}
```
adds three columns for integer entries (`"lhs"`, `"rhs"` and
`"expected`") and one for strings to the table. Make sure to provide a
_unique_ name for each column. The name must satisfy the same rules as
all C++ variable names.

After adding the columns, the `drtest::addRow` function may then be used
to populate the table:
```cpp
drtest::addRows(
    "row description", 
    1stColumnEntry,
    2ndColumnEntry,
    ...
  );
```
For example:
```cpp
DRTEST_DATA(someTestWithTable)
{
  // ...

  drtest::addRow(
      "Small numbers",  // Description of the row.
      3,  // lhs
      4,  // rhs
      7,  // expected
      std::string{"foo"}  // randomStuff
    );
  drtest::addRow(
      "Large numbers",
      1593,
      2478,
      4071,
      std::string{"bar"}
    );
  drtest::addRow(
      "This test will fail",
      2,
      2,
      5,
      std::string{"0>0"}
    );
}
```

Thus, the test table for `someTestWithTable` is initialized and
populated. To access the table in the test, use
```cpp
DRTEST(Type, column_name)
```
Note the lack of double quote in `column_name`!

Thus, to get the test data from above, do
```cpp
DRTEST_TEST(someTestWithTable)
{
  DRTEST_FETCH(int, lhs);
  DRTEST_FETCH(int, rhs);
  DRTEST_FETCH(int, expected);
  DRTEST_FETCH(std::string, randomStuff);

  // ...
}
```
Now you can use `lhs`, `rhs`, etc. as though they were C++ variables,
and **DrMock** will execute all statements involving these for every row
of the test table.  For example,
```cpp
DRTEST_TEST(someTestWithTable)
{
  // ...

  DRTEST_COMPARE(lhs + rhs, expected);
  DRTEST_ASSERT(randomStuff.size() > 2);
}
```
The first of these tests will check `3 + 4 == 7`, `1593 + 2478 == 4071`
and `2 + 2 == 5`. In case of failure, each of these will be displayed as
individual tests.

### Running the tests

Do `make`. This should yield the following:

```console
    Start 1: basicTest
1/1 Test #1: basicTest ........................***Failed    0.00 sec
TEST   someTest
PASS   someTest
TEST   anotherTest
PASS   anotherTest
TEST   exceptionTest
PASS   exceptionTest
TEST   someTestWithTable, Small numbers
PASS   someTestWithTable, Small numbers
TEST   someTestWithTable, Large numbers
PASS   someTestWithTable, Large numbers
TEST   someTestWithTable, This test fails
*FAIL  someTestWithTable, This test fails (109):
    (lhs + rhs) 4
    (expected) 5
****************
1 FAILED


0% tests passed, 1 tests failed out of 1

Total Test time (real) =   0.01 sec

The following tests FAILED:
	  1 - basicTest (Failed)
Errors while running CTest
make: *** [default] Error 8
```

In the test source `basicTest.cpp`, all tests passed except one:
```console
TEST   someTestWithTable, This test fails
*FAIL  someTestWithTable, This test fails (109):
    (lhs + rhs) 4
    (expected) 5
```
**DrMock** prints the failed test's name (`someTestWithTable`), the row
of the failed subtest (`This test will fail`, if the test uses test
tables), and the offending line (`89`). Furthermore, as the test used
`DRTEST_COMPARE`, the objects that failed the comparison are printed,
along with their variable name.

Change the test table so that the test check if `2 + 2 == 4` instead of
`2 + 2 == 5` and run `make` again:

```console
    Start 1: basicTest
1/1 Test #1: basicTest ........................   Passed    0.00 sec

100% tests passed, 0 tests failed out of 1

Total Test time (real) =   0.01 sec
```

### Caveats

#### Commas in macro arguments

The error
```console
error: too many arguments provided to function-like macro invocation
```
can be caused by not properly isolating commas occuring in macro
arguments. For example,
```cpp
DRTEST_ASSERT(
    std::vector<int>{0, 1, 2} 
    == 
    std::vector<int>{3, 4, 5}
  );
```
will throw this error. The macro will think it is called with the
arguments `std::vector<int>{0`, `1`, `2} == std::vector<int>{3`, etc...
To solve this, use parens `()` to encapsulate these commas:
```cpp
DRTEST_ASSERT(
    (std::vector<int>{0, 1, 2}) 
    == 
    (std::vector<int>{3, 4, 5})
  );
```

#### Implicit conversion in test tables

Implicit conversion doesn't work when adding rows to test tables.
Attempting implicit conversion when calling `drtest::addRow` will result
in a `type mismatch` error. For example,
```cpp
drtest::addRow(
    "Small numbers",
    3,
    4,
    7,
    "foo"
  );
```
will raise such an error, as the fourth column's type is defined as
`std::string`, yet a null-terminated C string was passed as argument.

## samples/mock

This sample demonstrates the basics of **DrMock**'s mock features.

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

### Introduction

**DrMock** generates source code of mock objects from _interfaces_. The
definition of the notion of _interface_ is given [below](#interface).
Let's introduce the concept with an example from Martin Fowler's 
[Mocks Arent' Stubs](https://martinfowler.com/articles/mocksArentStubs.html).

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

### Setup

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

A detailed documentation may be found at the end of the
subsection.

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

<details><summary>DrMockModule documentation</summary>
<p>

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
#### `TARGET`
  The name of the library that is created.

#### `HEADERS`
  A list of header files. Every header file must match the regex
  provided via the `IFILE` argument.

#### `IFILE`
  A regex that describes the pattern that matches the project's
  interface header filenames. The regex must contain exactly one
  capture group that captures the unadorned filename. The default
  value is ``I([a-zA-Z0-9].*)"`.

#### `MOCKFILE`
  A string that describes the pattern that the project's mock object
  header filenames match. The string must contain exactly one
  subexpression character `"\\1"`. The default value is `"\\1Mock"`.

#### `ICLASS`
  A regex that describes the pattern that matches the project's
  interface class names. The regex must contain exactly one capture
  group that captures the unadorned class name. Each of the specified
  header files must contain exactly one class that matches this regex.
  The default value is `IFILE`.

#### `MOCKCLASS`
  A string that describes the pattern that the project's mock object
  class names match. The regex must contain exactly one subexpression
  character `"\\1"`. The default value is `MOCKFILE`.

#### `GENERATOR` 
  A path to the generator script of DrMock. Default value is the
  current path.

#### `LIBS`
  A list of libraries that `TARGET` is linked against. Default value
  is an empty list.

#### `QT`
  If `QT` is set, the `HEADERS` will be added to the sources of
  `TARGET`, thus allowing the interfaces that are Q_OBJECT to be
  mocked. Default value is `OFF`.
  
#### `INCLUDE`
  A list of include path's that are required to parse the `HEADERS`.
  Default value is an empty list.

#### `FRAMEWORKS`
  A list of macOS framework path's that are required to parse the
  `HEADERS`. Default value is an empty list.
</p>
</details>

### Using the mock object

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
By calling the `mock` method of `WarehouseMock`, you can define the
behavior of the mock object. For instance, to define the behavior of
`remove`, you call `warehouse->mock.remove()`. This returns a `Method`
object onto which `Behavior`s may be pushed using `push`.  Here, the
mock object is instructed to expect the call `remove("foo", 2)` _exactly
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
DRTEST_COMPARE(order.filled(), true);
```

**Note.** When verifying the mock object, the `Behaviors` are expected
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

### Running the tests

Do `make` to run the tests. The following should occur:
```console
    Start 1: OrderTest
1/1 Test #1: OrderTest ........................   Passed    0.00 sec

100% tests passed, 0 tests failed out of 1

Total Test time (real) =   0.01 sec
```

### Behaviors

Recall that a call like `warehouse->mock.remove()` returns a `Method`
object that owns a queue (called the `BehaviorStack`) onto which
`Behavior`s may be pushed using `Method::push`. 

Everytime the underlying `Method` is called, the 
This section briefly describes how `Behavior`
instances may be configured.

#### `times`

Every `Behavior` _persists_ for a number of calls made to the underlying
`Method` before it expires. Using `times(unsigned int)`, this number of
calls is set. The default value is `1`.

See also: [persists](#`persists`).

#### `persists`

Every `Behavior` _persists_ for a number of calls made to the underlying
`Method` before it expires. Using `persists` makes the `Behavior`
immortal - it will never expire, no matter how many calls are made.

See also: [times](#`times`).

#### `expects`

Use `expects(Args... args)` to instruct the `Behavior` to expect the
underlying method to be called with `args...` as arguments. 

#### `returns`

Use `returns(T&& result)` to instruct the `Behavior` to return `result`
on every method call that triggers it. Here `T` is the underlying
`Method`'s return value type.

#### `throws`

Use `throws(E&& exception)` to instruct the `Behavior` to throw
`exception` on every method call that triggers it.

### Details

#### Interface methods

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

#### Polymorphism

If an interface's method accepts or returns an `std::shared_ptr<B>` or
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

#### Operators

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

#### Changing nomenclature templates

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

#### Ignore order of behaviors

Recall that `Behavior`s are expected to occur in the order in which they
are pushed onto the `Method`'s `BehaviorStack`. This can be disabled by
calling `Method::enfore_order` as follows:
```cpp
warehouse->mock.remove().enforce_order(false);
```
If `enforce_order` is disabled, and the mocked method is called, the
first `Behavior` on the `BehaviorStack` that matches the method call is
triggered.

### Caveats

#### Raw pointers as parameters

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

### Fine print: Interface

Here's the definition of the notion of _interface_ in the context of a
call of `DrMockModule`:

## samples/qt

```
samples/qt
│   CMakeLists.txt
│   Makefile
│
└───src
│   │   CMakeLists.txt
│   │   IFoo.h
│   
└───tests
    │   CMakeLists.txt
    │   FooTest.cpp
```

This sample demonstrates how to use **DrMock** to mock an interface
`IFoo` that inherits from `QObject` and uses the `Q_OBJECT` macro. 

### Setup

Let's take a look at the changes make to `CMakeLists.txt`. 

```cmake
# samples/qt/CMakeLists.txt

# ...

find_package(DrMock COMPONENTS Core REQUIRED)
DrMockEnableQt()
find_package(Qt5 COMPONENTS Core REQUIRED)

# ...
```

To enable Qt for **DrMock**, the `DrMockEnableQt()` macro must be called
after finding the **DrMock** package. This macro enables `CMAKE_AUTOMOC`
and a CMake policy that allows mocking generated source code. Of course,
one must also find the Qt5 package.

When calling `DrMockModule`, the required Qt5 modules must now
bespecified using the keyword `QTMODULES`, as follows.  **DrMock** will
automatically link `TARGET` to the required libraries.

```cmake
DrMockModule(
  TARGET DrMockSampleQtMocked
  generator ${pathToMocker}
  QTMODULES
    Qt5::Widget
    # ...
  HEADERS
    IFoo.h
)
```

The `DrMockTest` call requires no changes for the use of Qt.

### Mocking a QObject

A class that is derived from `QObject`<sup>[3]</sup> and holds the `Q_OBJECT` macro in
the private section of its body may be mocked if it satisfies the
following rules:

* The only declarations in the interface shall be public methods, public
  slots, signals and type alias (template) declarations.

* All methods shall be declared pure virtual, with the exception of the
  signals.

* The interface shall not contain any conversion functions.

* If an operator is defined in the interface, the interface shall not
  have a method called `operator[SYMBOL]`, where `[SYMBOL]` is
  determined by the operator's symbol according to the table below.

* None of the interface's method shall be a volatile qualified method
  or a method with volatile qualified parameters.

[3]: Note that condition is satisfied if, for instance, the class is
  derived from `QWidget`, which in turn is derived from `QObject`.

The interface `IFoo` inhertis from `QWidget`, has a pure virtual slot
and a signal.

```cpp
class IFoo : public QWidget
{
  Q_OBJECT

public:
  virtual ~IFoo() = default;

public slots:
  virtual void someSlot(const std::string&) = 0;

signals:
  void someSignal(const std::string&);
};
```

To demonstrate the mock object, two instances of `FooMock` are made
and `someSignal` of `foo1` connected to the slot of `foo2`.

```cpp
QObject::connect(
    foo1.get(), &IFoo::someSignal,
    foo2.get(), &IFoo::someSlot
  );
```

Before that, `foo2` is instructed to expect a call of `someSlot` with
the argument `"foo"`. After `foo1` emits `someSignal`, this may be
verified by `foo2`:

```cpp
emit foo1->someSignal("foo");

DRMOCK_ASSERT(foo2->mock.verify());
```

### Running the tests

Do `make`. If everything checks out, this should return
```console
Test project /Users/malte/DrMock/samples/qt/build
    Start 1: FooTest
1/1 Test #1: FooTest ..........................   Passed    0.02 sec

100% tests passed, 0 tests failed out of 1

Total Test time (real) =   0.02 sec
```
