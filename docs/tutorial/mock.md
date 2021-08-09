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
* [The structure and use of **DrMock** mock objects](#the-structure-and-use-of-drmock-mock-objects)
  + [The user handle](#the-user-handle)
  + [`Method` objects and `Behavior` objects](#method-objects-and-behavior-objects)
  + [`Behavior` configuration](#behavior-configuration)
  + [Combining config calls](#combining-config-calls)
  + [Examples](#examples)
* [Details](#details)
  + [Accessing overloads](#accessing-overloads)
  + [Interface methods](#interface-methods)
  + [Polymorphism](#polymorphism)
  + [Operators](#operators)
  + [Changing nomenclature templates](#changing-nomenclature-templates)
  + [Ignore order of behaviors](#ignore-order-of-behaviors)
  + [drmock_library documentation](#drmockmodule-documentation)
  + [Macros](#marcos)
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

This project requires an installation of **DrMock** in `build/install/`
or a path container in the `CMAKE_PREFIX_PATH` variable. If your
installation of **DrMock** is located elsewhere, you must change the
value of `CMAKE_PREFIX_PATH`.

## Introduction

Let's introduce the concept of _mock_ with an example from M. Fowler's
_Mocks Aren't Stubs_ [1]:
At an online shop, customers fill out an `Order` with the commodity
they'd like to order and the order quantity. The `Order` may be filled
from a `Warehouse` using `fill`. But `fill` will fail if the `Warehouse`
doesn't hold the commodity in at least the quantity requrested by the
customer. We want to test this interaction between `Order` and
`Warehouse`, and, assuming that the unit `Warehouse` has already been
tested, will use a mock of `Warehouse` for this purpose. This will help
decouple the correctness of the implementation of `Order::fill` from 
the correctness of the implementation of `Warehouse`.

For the sake of clarity, we separte the interface and implementation of
the warehouse functionality into two classes `IWarehouse` (the
interface) and `Warehouse` (the implementation, which is a subclass of
`IWarehouse`):

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

(Note that `remove` returns a `bool`. If removing fail due to too large a
quantity being requested, this should be `false`. Otherwise, `true`.)

**DrMock** generates source code of _mock implementations_ from C++
classes called _interfaces_ (the class that you wish to mock, in this
case `IWarehouse`).
We have some minor requirements regarding the structure of the
interface. You can review them in the [**DrMock**
specification]. This is fine print - most classes will
qualify. The most important requirements are summarized in [Fine print].
Unless you encounter a problem, there is no reason to look deeper into
these details.

The code generations is done using the
[drmock-generator](https://github.com/DrCpp/drmock-generator), which is
developed in Python and available via `pip`. The _generator_ is called
at compile time to generate the source code of the mock implementation.
During runtime (for example during a test), the exact behavior of the
implementation may then be configured using the API specified in detail
below. For example:

```cpp
DRTEST_TEST(success)
{
  drmock::samples::Order order{"foo", 2};

  auto warehouse = std::make_shared<drmock::samples::WarehouseMock>();
  // Inform `warehouse` that it should expect an order of two units of
  // `"foo"` and should return `true` (indicating that those units are
  // available).
  warehouse->mock.remove().push()
      .expects("foo", 2)  // Expected arguments.
      .times(1)  // Expect **one** call only.
      .returns(true);  // Return value.

  order.fill(warehouse);
  // Check that `remove` was called with the correct arguments.
  DRTEST_ASSERT(warehouse->mock.control.verify());
  // Check that the return value of `filled` is correct.
  DRTEST_ASSERT(order.filled());
}
```

We will explain the function of this API later. If you are curious about
the implementation of the API, check out the [**DrMock** specification]
or the [drmock-generator] source code.

*Note.*
The requirement that the interface be _abstract_ is removed in
**DrMock** 0.6. This means that the separation of interface `IWarehouse`
and implementation `Warehouse` is not required anymore. Instead,
`Warehouse` could serve both as interface and as implementation.
Nevetheless, we will proceed using `IWarehouse` as interface.

## Setup

To instruct `drmock-generator` to create source code for a mock of
`IWarehouse`, the macro `drmock_library` is used.
```cmake
# src/CMakeLists.txt

add_library(DrMockSampleMock SHARED
    Order.cpp
    Warehouse.cpp
)

drmock_library(
    TARGET DrMockSampleMockMocked
    HEADERS
        IWarehouse.h
)
```
The `HEADERS` parameter specifies the interface to be mocked.
The `drmock_library` macro generates the source code of the mocks and
compiles them into a dynamic library whose name is specified by
`TARGET`. Other parameters of `drmock_library` are discussed later.

As discussed in the last chapter, we must now use the `LIBS` parameter
of `drmock_test` to link the test against the binary which contains the
mock code:
```cmake
# tests/CMakeLists.txt
drmock_test(
    LIBS
        DrMockSampleMock
        DrMockSampleMockMocked
    TESTS
        OrderTest.cpp
)
```

*Note.*
The `drmock_library` macro's implementation is a wrapper of
`drmock-generator`. It hides some of the parameters of
`drmock-generator` which are tedious for the user to manage (type
`drmock-generator --help` in the terminal to see a full list of the
parameters), but it does
so at the cost of forcing you to use CMake as build tool. If can't or
won't use CMake, you have two options. You could manage the
`drmock-generator` call manually. This is similar to writing you own
Makefile instead of using a modern build tool and is not recommended.
The other is to [contribute](CONTRIBUTING.md) by implementing
`drmock_library` for the build tool you wish to use.

## Including the mock object header

By default, the mock of `I*` is called `*Mock` and placed in the same
namespace as the interface, but you may change this pattern if you like.
The header file of the mock object is included with `#include
"mock/WarehouseMock.h"`.

For every file under `HEADER`, say `path/to/IFoo.h`, **DrMock**
generates header and source files `FooMock.h` and `FooMock.cpp`, which
may be included using the path `mock/path/to/IFoo.h`. In these files,
the mock class `FooMock` is defined. The path is _relative to the
current CMake source dir_. Thus, calling `drmock_library` from anywhere
but `src/CMakeLists.txt` is bound to result in odd include paths.

## Using the mock object

Let's take a look at the header of `Order`:
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
Note that `fill` takes a reference to a `std::shared_ptr` to allow the
use of polymorphism. You could use virtually any type of (smart) pointer
in place of `std::shared_ptr`, or even a reference if your interface is
not abstract.

Here's the straightforward implementation of the `fill` method:
```cpp
void
Order::fill(const std::shared_ptr<IWarehouse>& wh)
{
  filled_ = wh->remove(commodity_, quantity_);
}
```
Thus, the order will attempt to remove the requested amount of units of
the commodity from the warehouse and set `filled_` accordingly.

How do we test `fill`? Let's look at the `success` test of
`OrderTest.cpp` (you've already seen it in the introduction):
```cpp
DRTEST_TEST(success)
{
  drmock::samples::Order order{"foo", 2};

  auto warehouse = std::make_shared<drmock::samples::WarehouseMock>();
  // Inform `warehouse` that it should expect an order of two units of
  // `"foo"` and should return `true` (indicating that those units are
  // available).
  warehouse->mock.remove().push()
      .expects("foo", 2u)  // Expected arguments.
      .times(1)  // Expect **one** call only.
      .returns(true);  // Return value.

  order.fill(warehouse);
  // Check that `remove` was called with the correct arguments.
  DRTEST_ASSERT(warehouse->mock.control.verify());
  // Check that the return value of `filled` is correct.
  DRTEST_ASSERT(order.filled());
}
```

The third line (`warehouse->mock.remove().push() ...`) uses the API of
**DrMock** to configure the behavior of `warehouse`. Every mock object
contains a public member `mock` of type `DRMOCK_OBJECTIWarehouse`, whose
source code is generated alongside that of `WarehouseMock`. This
_user handle_ lets the user define the expected behavior of the
mock obejct:

```cpp
auto warehouse = std::make_shared<drmock::samples::WarehouseMock>();
warehouse->mock.remove().push()
    .expects("foo", 2u)
    .times(1)
    .returns(true);
```

To define the behavior of `remove`, we call `warehouse->mock.remove()`.
This returns a `drmock::Method` object which controls the behavior of
`warehouse->remove`. By default, the a `drmock::Method` object has an
internal queue onto which `drmock::Behavior` objects may be pushed.

Here, we push one element, which instructs the mock object to _expect_
a call `warehouse->remove("foo", 2)`, to expect it _exactly_ one time,
and to return `true` if `warehouse->remove("foo", 2)` is called.
If an unexpected call is made (or too many or too few expected calls),
then the `Method` object will log an error.

*Note.* `push`, `expects`, `times`, `returns`, etc. return a reference
to the pushed `Behavior` object, allowing us to use call chaining when
configuring a mock object.

Now that the behavior of `warehouse` is defined, the order for two units
of `foo` is filled from the warehouse. Judging from the implementation
of `fill`, this should call `warehouse->remove("foo", 2)`. And, as
we defined earlier (by calling `returns(true)`), removing two units of
`foo` should "succeed".

Whether the expected behavior was observed or not may be verified using
the following call:
```cpp
DRTEST_ASSERT(warehouse->mock.remove().verify());
```
This is equivalent to:
```cpp
DRTEST_VERIFY_MOCK(warehouse->mock.remove());
```
After verifying the mock, we check if the `filled` method returns the
correct value:
```cpp
DRTEST_ASSERT(order.filled());
```

*Note.* When verifying the mock object, the `Behavior` objects are
expected to occur in the order in which they were pushed. More on that
later.

The second test runs along the same lines. The customer places an order
for two units of foo, but this time a failure is simulatedd:
```cpp
auto warehouse = std::make_shared<drmock::samples::WarehouseMock>();
warehouse->mock.remove().push()
    .expects("foo", 2u)
    .times(1)
    .returns(false);
```
Once again, `warehouse->mock` is verified, and `order.filled()` is now
expected to return `false`.

*Note.* The terminology is not identical to that used in the **DrMock**
specification. In the specification, the user handle is simply
called _mock object_, and the mock object is called the _mock
implementation_.


## Running the tests

Do `make` to run the tests. The following should occur:
```
    Start 1: OrderTest
1/1 Test #1: OrderTest ........................   Passed    0.00 sec

100% tests passed, 0 tests failed out of 1

Total Test time (real) =   0.01 sec
```

## **DrMock** API

Every virtual method `f` (`add`, `remove`, ...) of `IWarehouse`
corresponds to a `drmock::Method` object which can be obtained by
calling `warehouse->mock.f()` (overloads are resolved using template
parameters, see [Accessing overloads]). A detailed documentation of `Method` may be
found in [Method.h](../../src/DrMock/mock/Method.h).

The behavior of `f` is controlled by the behavior of the underlying
`Method` object. In fact, the implementation of `WarehouseMock::f` uses
`Method::call`, which simulates one function call
(for details, see the [**DrMock** specification]).

By default, the behavior of a `Method` object is controlled by a
`BehaviorQueue` object (_the queue_; in the next chapter, we will use a
`StateBehavior`, instead). The queue is empty when the `BehaviorQueue`
is constructed. As we saw above, new `Behavior` objects may be pushed
onto the queue using `BehaviorQueue::push` or the convenience method
`Method::push`. Both methods return a reference to the newly pushed
`Behavior`.

In other words, the following code configures an instance of `Behavior`
on the queue of `warehouse`:
```cpp
warehouse->mock.remove().push().  // drmock::Behavior&
    // Some config calls...
```

### `Behavior` API

A `Behavior` object (_behavior_) is the smallest unit used to simulate a
method.
Every behavior can _expect_ a call with a specified _input_
(a set of arguments that match the parameter types), and _produce_ a
_result_ when called. The result can be a return value and/or a Qt
signal emit (if you're not familiar with this, ignore it), _or_ an
exception pointer (simulating a thrown exception). Returning a value and
emitting a signal may both occur, but exceptions are exclusive.

Furthermore, every `Behavior` object has a _life span_, which means that
it can only `produce()` a fixed number of results (may be infinite).
Once the fixed number of productions is reached, the behavior no longer
_persists_. The results of the production, as well as the life span must
be configured by the user.

For example,
```cpp
warehouse->mock.remove().push()
    .expects("foo", 2u)
    .times(1)
    .returns(true);
```
pushes a new `Behavior` onto the `BehaviorQueue`, then configures that
`Behavior` to expect the input `("foo", 2)`, to persists for exactly one
production, and to produce `true`.

The available methods for configuring a `Behavior` object are the
following:

* `Behavior& expects()` - Expect _any_ input (this is the default)
* `Behavior& expects(detail::Expect<Args>... args)` - Expect input that
  matches `args` (`Args...` are the parameters of `f`); the `i`th
  element may be an instance of `Args[i]` or an instance of
  `std::shared_ptr<IMatcher<Args[i]>>`
* `template<typename... Deriveds> Behavior& polymorphic()` - Set the
  `Derived...` type pack for the matching handler
* `template<typename T> Behavior& return(T&& result)` - Produce return
  value `result` when called
* `template<typename E> Behavior& throws(E&& e)` - Produce exception
  value `e` when called
* `template<typename E> Behavior& emits(void (Class::*signal)(SigArgs...), SigArgs&&... args)` -
  Produce a Qt signal emit when called
* `Behavior& times(unsigned int count)` - Expect _exactly_ `count`
  productions (function calls)
* `Behavior& times(unsigned int min, unsigned int max)` - Expect
  production count in `[min, max]`
* `Behavior& persists()` - Expect _any_ number of productions and make
  `this` immortal

The default expected number of productions is `1`.

Some of the configuration calls may be combined, while others create
conflicts:

* Multiple calls to the same function are not allowed

* `returns` and `emits` may be combined (the method will emit first,
  then return, of course)

* `throws` may not be combined with `returns` or `emits`

For example:
```cpp
behavior
   .returns(123)
   .emits(&Dummy::f, 456)  // Ok, returns and emits are parallel.
   .throws(std::logic_error{""})  // Not ok, overriding previous behavior.
   .returns(456)  // Not ok, overriding previous behavior.
```

For details, refer to [Behavior.h](../../src/DrMock/mock/Behavior.h)
Matching and polymorphism are described in [Matching and polymorphism].


### `BehaviorQueue` API

The queue controls the behavior of `f` as follows. Let `b` be the front
element of the queue. When `f` is called with `args...`, the following
occurs:

- If `b` expects `args...`, then the result of `b` occurs (`f` returns
  a value and/or emits signal, or throws exception)
- Otherwise, the call _fails_ (see [Failure] for details)
- If `b` has reached the maximum number of productions (`b` is
  _exhausted_), then `b` is popped off the queue

If the queue is empty and `f` is called, the call fails (see [Failure]).

Furthermore, `BehaviorQueue` exposes an API for configuration:

* `void enforce_order(bool)` - When set to `false`, the order of the
  queue is ignored and the entire queue is searched for a matching
  behavior on every call
* `template<typename... Deriveds> void polymorphic()` - Set the
  `Deriveds...` type pack of all future behaviors _and_ all behaviors
  already enqueued to the default, with the specified polymorphic type
  (see [Matching and polymorphism] for details)


### Failure

You can check the correctness of a `Method` object by calling
`verify()`, which returns `false` if _any_ function call of `f` has
failed. You can use `makeFormattedErrorString()` to get a report of all
errors that have occured.

If `Method::call` fails, **DrMock** will try to gracefully recover:

* If the return type is default constructible, return a default
  constructed value.

* If the return type is `void`, return.

* Otherwise, `std::abort`.

The `DRTEST_VERIFY_MOCK` macro calls `verify()` and prints
`makeFormattedErrorString()` if it returns `false`.

*Note.* A failed execution is caused by unexpected behavior of any of
the components that access the mock object, or by an incorrect
configuration of the queue.


### Examples

See [samples/example](../../samples/example) for a sample that
demontrates the use of the functions discussed above.


## Details

### The controller object

By default, `mock` contains a member `control` of type
`drmock::Controller`, which holds all the `Method` objects. This object
provides diagnostic methods, some of which only make sense in the
context of `StateBehavior` (see the next chapter).

- You can verify _all_ methods of a mock object at once by calling
  ```cpp
  warehouse->mock.control.verify()
  ```
  or, equivalently,
  ```cpp
  DRTEST_VERIFY_MOCK(warehouse->mock);
  ```
  Beware! Unconfigured methods will result in a failed test if this macro
  is used.

- If any methods have failed, use `makeFormattedErrorString` to obtain a
  comprehensive summary of the errors that have occured


### Accessing overloads

Consider the following interface:
```cpp
class IBar
{
public:
  virtual ~IBar() = default;

  virtual int f() = 0;
  virtual int f() const = 0;
  virtual int f(int) = 0;
  virtual int f(float, const std::vector<int>&) const = 0;

  virtual int& g(std::vector<int>&) = 0;
  virtual const int& g(std::vector<int>&) const = 0;

  virtual float h() const = 0;
  virtual float h(int, float) const = 0;
};
```

How do we access the methods here? We can't just do `bar->mock.f()` -
**DrMock** wouldn't know which overload you want!
Instead, template parameters must be used:

* If all members of the overload have the same const/reference
  qualifiers (but different parameters), then `bar->mock.f<Ts...>()`
  returns the `Method` object corresponding to the overload with
  parameters `Ts...`.
  For example, `bar->mock.h<>()` selects `float h() const`, and
  `bar->mock.h<int, float>()` selects `float h(int, float) const`.

* If all members of the overload have the same parameters (but different
  const/reference qualifiers), then use `drmock::Const`,
  `drmock::LValueRef` and `drmock::RValueRef` as template parameters
  to select methods (in this order, i.e. `drmock::Const` _before_
  references).
  For example, `bar->mock.g<>()` selects `int& g(std::vector<int>&)`,
  and `bar->mock.g<drmock::Const>()` selects
  `const int& g(std::vector<int>&) const`.

* If members of the overload have varying parameters and qualifiers,
  then use parameter types `Ts...` followed by qualifiers types (see
  above) to select overloads.
  For example, `bar->mock.f<int>()` selects `int f(int)`, and
  `bar->mock.f<float, const std::vector<int>&, drmock::Const>()` selects
  `f(float, const std::vector<int>&) const`.

Below is a complete example (see
[BarTest.cpp](../../samples/example/tests/BarTest.cpp)):

```cpp
DRTEST_TEST(params_and_qualifiers)
{
  auto bar = std::make_shared<BarMock>();
  bar->mock.f<>().push()
      .expects()
      .returns(1);
  bar->mock.f<drmock::Const>().push()
      .expects()
      .returns(2);
  bar->mock.f<int>().push()
      .expects(3)
      .returns(3);
  bar->mock.f<float, const std::vector<int>&, drmock::Const>().push()
      .expects(1.2f, {1, 2, 3})
      .returns(4);

  DRTEST_ASSERT_EQ(
      bar->f(),
      1
    );
  DRTEST_ASSERT_EQ(
      std::const_pointer_cast<const BarMock>(bar)->f(),
      2
    );
  DRTEST_ASSERT_EQ(
      bar->f(3),
      3
    );
  DRTEST_ASSERT_EQ(
      bar->f(1.2f, {1, 2, 3}),
      4
    );
}
```

### Matching and polymorphism

Recall that
```cpp
Behavior& Behavior::expects(Expect<Args>... args)
```
defines a behavior to produce only when called with arguments that
_match_ `args...`. But the parameters of `expects` are
`Expect<Args>...`, which is a variant type. Making use of implicit
conversion, the type of the `i`th element of `args...` may be `Args[i]`,
but it may also be a `std::shared_ptr<IMatcher<Args[i]>>`.

The definition of `IMatcher` is simple:
```cpp
template<typename Base>
class IMatcher
{
public:
  virtual ~IMatcher() = default;
  virtual bool match(const Base& x) const = 0;
};
```
The `match` method matches the object `x` against some pattern. For
example, [Equal](../../src/DrMock/mock/Equal.h) implements `IMatcher`
and checks if `x` is equal to the element that the instance of `Equal`
was constructed with:
```cpp
Equal<int> equal_to_five{5};
DRTEST_ASSERT(equal_to_five->match(5));
DRTEST_ASSERT(not equal_to_five->match(2 + 2));
```
(More on the second template parameter of `Equal` later.)

By default, if `expects` is called with an element `args` of type
`Args[i]` instead of `std::shared_ptr<IMatcher<Args[i]>>` in `i`th
position (this is called _raw input_), it is wrapped in an instance of
`std::shared_ptr<Equal<Args[i]>>` by the _matching handler_. Internally,
the behavior only works with matchers. When called, it checks if the
input in each position _matches_ the corresponding matcher and produces
only if this is the case.

By letting us pass an `std::shared_ptr<IMatcher<Args>>`, the API
allows us to specify more complex matching behavior. For example,
[AlmostEqual](../../src/DrMock/mock/AlmostEqual.h) implements typical
floating point approximation logic and may be used to define a
production for a small range of floating point numbers, compensating the
lack of absolute precision in floating point math:

```cpp
b.expects("foo", drmock::almost_equal(1.0f));
```

This expects the arguments to be `"foo"` and a floating point number
almost equal to `1.0f`.

Users may implement
their own matchers by implementing `IMatcher`.

The second template parameter of `Equal` allows the user to take
polymorphism into account. Consider the following situation:

```cpp
class Base
{
  // ...
};

class Derived : public Base
{
public:
  Derived(int x) : x_{x} {}
  bool operator==(const Derived& other) const
  {
    return x_ == other.x_;
  }

  // ...

private:
  int x_;
  // ...
};

// IFoo.h

class IFoo
{
public:
  ~IFoo() = default;

  void func(std::shared_ptr<Base>) = 0;
};
```

Suppose we want to configure `FooMock` to expect
`ptr = std::make_shared<Derived>(1)`. This is done using
`Equal<std::shared_ptr<Base>, std::shared_ptr<Derived>>`, which applies
`dynamic_pointer_cast<Derived>` before matching the actual value against
the expected value:

```cpp
DRTEST_TEST(polymorphic)
{
  FooMock foo{};
  auto ptr = std::make_shared<Derived>(1);
  foo.mock.func().push()
      .expects(drmock::equal<Base, Derived>(ptr));
}
```

To improve readability, we can use `polymorphic<Deriveds...>()` to
replace the default matching handler with one that wraps raw input in
`std::shared_ptr<Equal<std::shared_ptr<Base>, std::shared_ptr<Derived>>>`
instead of the default
`std::shared_ptr<Equal<std::shared_ptr<Base>>>`:

```cpp
DRTEST_TEST(polymorphic)
{
  FooMock foo{};
  auto ptr = std::make_shared<Derived>(1);
  foo.mock.func().push()
      .polymorphic<std::shared_ptr<Derived>>()
      .expects(ptr);
}
```

Note that this call will not change the expected value. Only the effect
of future calls to `Behavior::expect` is changed. Note that
`BehaviorQueue::polymorphic` sets the matching handler for previously
enqueued behaviors and future behaviors.

Polymorphism is supported for `std::shared_ptr` and `std::unqiue_ptr`.
However, there is a requirement on the types `(B, D)` used as
template parameters for `Equal` (in the example above,
`std::shared_ptr<Base>` and `std::shared_ptr<Derived>`).
`D` need not derive from `B`. Instead, `(B, D)` must
satisfy one of the following requirements (such a pair is called
_comparable_):

1. `D` is not abstract, inherits from `B` and implements `bool
   operator==(const D&) const`
2. `B` is a `shared_ptr<T>` (or `unique_ptr<T>`) and `D` is a
   `shared_ptr<U>` (or `unique_ptr<U>`), and `(T, U)` satisfies 1., 2.
   or 3.
3. `B` is a `tuple<Ts...>` and `D` is `tuple<Us...>` of the
   same length `n` so that for all `i=0, ..., n-1`, `(Ts[i], Us[i])`
   satisfies 1., 2. or 3.

`(B, D)`-_equality_ is then defined recursively: Let `x` and
`y` be instances of `B`.

- In case 1., `x` is `(B, D)`-equal to `y` if `x == y`.
- In case 2., `x` is `(B, D)`-equal to `y` if they can successfully be
  cast from `std::shared_ptr<T>` to `std::shared_ptr<U>`, yielding
  elements `xder` and `yder`, and if `xder` is `(T, U)`-equal to `yder`
- In case 3., `x = (x0, ..., xN)` is `(B, D)`-equal to `y = (y0, ..., yM)` if
  `N == M` and for each `i=0, ..., N`, `x[i]` is `(B[i], D[i])` equal to
  `y[i]`

If a parameter is not comparable (for example, a class from a
third-party library that you do not have any control over), this
parameter can forcibly made comparable using the `DRMOCK_DUMMY` macro,
at least if `bool operator==(const Foo&) const` is not deleted.


### Floating point comparison

Regarding `almost_equal`, the algorithm of comparison is the same as that
defined in the chapter [basic.md](basic.md). The precision of the
comparison may be set using 

```cpp
template<typename T> drmock::almost_equal(T expected, T abs_tol, T rel_tol)
```

or by `#define`-ing `DRTEST_*_TOL`, as described in
[basic.md](basic.md).

Beware of using the correct types! The following call is not allowed:

```cpp
b.expects("foo", drmock::almost_equal(1.0));  // Using double, not float...
```

Failing to following this rule will lead to a potentially confusing
error message like this:

```shell
/Users/malte/drmock/tests/Behavior.cpp:424:5: error: no matching member function for call to 'expects'
  b.expects("foo", almost_equal(1.0), poly1);
  ~~^~~~~~~
/Users/malte/drmock/src/mock/Behavior.h:76:13: note: candidate function not viable: no known conversion from 'std::shared_ptr<ICompare<double>>' to 'detail::expect_t<float>' (aka 'variant<float, std::shared_ptr<ICompare<float>>>') for 2nd argument
  Behavior& expects(detail::expect_t<Args>...);
            ^
/Users/malte/drmock/src/mock/Behavior.h:77:38: note: candidate function template not viable: no known conversion from 'std::shared_ptr<ICompare<double>>' to 'detail::expect_t<float>' (aka 'variant<float, std::shared_ptr<ICompare<float>>>') for 2nd argument
  template<typename... Ts> Behavior& expects(detail::expect_t<Args>...);
                                     ^
/Users/malte/drmock/src/mock/Behavior.h:70:59: note: candidate function template not viable: requires 0 arguments, but 3 were provided
  std::enable_if_t<(std::tuple_size_v<T> > 0), Behavior&> expects();
```


### Operators

Mocking an operator declared in an interface is not much different from
mocking any other method, only the way in which the mocked method is
accessed from the mock object changes. Instead of
```cpp
foo->mock.operator*().expects(/* ... */);
```
(which is illegal), you must use
```cpp
foo->mock.operatorAst().expects(/* ... */);
```
The illegal tokens are replaced with a designator describing the
operators symbol. The designators for C++'s overloadable operators are
found in the table below.

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


### Mocking non-abstract classes

As of version `0.6`, the [**DrMock** specification] no longer requires
that the interface be abstract. All pure and non-pure virtual methods
are now (re-)implemented using the code generated by `drmock-generator`.
The mock object implements a forwarding constructor which forwards any
parameters to the base class constructor.


### Dummy types

`mock/MockMacros.h` currently declares the `DRMOCK_DUMMY` macro.
Using `DRMOCK_DUMMY(Foo)` defines a trivial `operator==` as follows:
```cpp
inline bool operator==(const Foo&, const Foo&)
{
  return true;
}
```
This can be used to make all third-party classes that occur as method
parameters in an interface comparable.

However, you may not want this to end up in production code. Header
files created by `drmock-generator` define the `DRMOCK` macro for the
purpose of protecting other `DRMOCK` macros from the preprocessor in
production code:
```cpp
#ifdef DRMOCK
DRMOCK_DUMMY(QQuickWindow)
#endif
```

**Beware!** This macro must be used _outside_ of any namespace,
and the parameter `Foo` must specify the full namespace of the target class.
Also note that adding a semicolon `;` at the end of the macro call
may (depending on your compiler) result in an
`extra ‘;’ [-Werror=pedantic]` error.

If `Foo::operator==` is deleted
_and_ you control the source code of `Foo`,
you can ignore the delete in test/mock code by using the `DRMOCK` macro:
```cpp
class Foo
{
#ifndef DRMOCK
  bool operator==(const Foo&) const = delete;
#endif

  /* ... */
}

#ifdef DRMOCK
DRMOCK_DUMMY(Foo)
#endif
```


## `drmock_library` and `drmock-generator`

Recall that the CMake macro `drmock_library` calls the Python script
`drmock-generator`, which creates the source code of mock objects, and
compiles the source code into a C++ library.

```
drmock_library(
    TARGET <target>
    HEADERS header1 [header2 ...]
    [IFILE <ifile>]
    [MOCKFILE <mock_file>]
    [ICLASS <iclass>]
    [MOCKCLASS <mockclass>]
    [LIBS lib1 [lib2 ...]]
    [QTMODULES module1 [module2 ...]]
    [INCLUDE include1 [include2 ...]]
    [FRAMEWORKS framework1 [framework2 ...]]
    [OPTIONS option1 [option2 ...]]
    [FLAGS flag1 [flag2 ...]]
)
```

You may find detailed documentation in
[DrMockMacros.cmake](../../cmake/DrMockMacros.cmake).

The name of the output of `drmock-generator` is determined by matching
each input filename (with extensions removed) against `<ifile>` and
replacing the subexpression character `\\1` in `<mockfile>` with the
content of the unique capture group of `<ifile>`, then adding on the
previously removed file extension. The class name of the mock object is
computed in analogous fashion.

The `<ifile>`/`<iclass>` regex must contain exactly one capture group, and
the `<mockfile>`/`<mockclass>` must contain exactly one backreference
character `\1`.

The `INCLUDE`, `LIBS`, `QTMODULES` and `FRAMEWORK` parameters are used
to specify lists of additional includes, dynamic libraries to link
against, Qt modules to use and macOS framework paths to use.

If you run `drmock-generator --help`, you will see that
`drmock-generator` offers some customizability. `drmock_library` exposes
these parameters through the `OPTIONS` and `FLAGS` parameters: The items
of the `OPTIONS` list are forwarded to `drmock-generator` directly. The
`FLAGS` parameter is for passing compiler flags to `drmock-generator`
(note that `clang` is the compiler that `drmock-generator` uses).
items of the `FLAGS` are forwarded to the `--flags` keyword of the
`drmock-generator` and used as `target_compile_options` for `TARGET`.

Only `--access`, `--namespace`, `--controller` and
`--clang-library-file` should be specified under `OPTIONS`.
`--input-class` and `--output-class` are derived from the regular
expressions passed to `drmock_library`. To specify `--flags`, use the
`FLAGS` parameter.


### Changing default values

Throughout this tutorial, a couple of default behaviors and settings
have been mentioned, such as the name of the controller object
`control`. Sometimes, you may wish to change these using the OPTIONS of
`drmock-generator`. For details, use `drmock-generator --help`.

* By default, `drmock-generator` mocks all virtual methods, regardless of
their access specifier. If you only wish to mock methods with a specific
access specifier, use `--access`. For example,
`--access public protected` mocks all non-private methods.

* By default, mock objects are placed in the same namespace as the
interface. If you instead wish to place them in a different namespace,
use `--namespace`. Use a leading `::` to specify a global namespace;
otherwise, the namespace you provide is considered to be _relative_ to
the namespace of the interface. For example, if
`outer::inner::Interface` is the interface and you specify
`--namespace ::path::to::namespace`, then the mock object is placed in
`::path::to::namespace`. If you specify `--namespace
path::to::namespace`, the mock object is placed in
`inner::outer::path::to::namespace`.

* `drmock_library` will try to detect the path to the `libclang` library
automatically, either through CMake's `find_library` or by checking the
environment variable `CLANG_LIBRARY_PATH`. Should this fail, you can
specify the path manually using `--clang-library-path`.

* If you wish to rename the `Controller` (this will be necessary if you
have a method called `control`), use `--control NAME` to specify a new
name for the member.



## Fine print

We mention some requirements for mocking an interface. For details, see
[**DrMock** specification].

### Method parameters

When declaring a method in an interface, all type references occuring in
the declaration of the parameters and return values must be declared
with their full enclosing namespace.

In other words, this is wrong:
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

### Forbidden names

When mocking `Interface`, observe the following rules:

- The interface's name must not contain the substring `DRMOCK`
- If the declaration contains an operator with symbol `SYMBOL`,
  then `Interface::` must not contain a method called
  `operator{designator(SYMBOL)}`, where `designator` is defined
  according to the table in [Operators]
- `Interface::` must not have a member `mock`
- `Interface::` must not have a member whose name contains the
  substring `DRMOCK`
- Every parameter type used in a virtual function must be comparable



## Bibliography

[1] [M. Fowler, _Mocks Aren't Stubs_](https://martinfowler.com/articles/mocksArentStubs.html)


<!-- Links -->

[**DrMock** specification]: ../spec.md
[drmock-generator]: https://github.com/DrCpp/drmock-generator
[OrderTest.cpp]: ../../samples/mock/tests/OrderTest.cpp
[Failure]: #failure
[Matching and polymorphism]: #matching-and-polymorphism
[Fine print]: #fine-print
[Accessing overloads]: #accessing-overloads
[Operators]: #operators
