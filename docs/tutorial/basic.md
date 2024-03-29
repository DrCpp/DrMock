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

# samples/basic

This sample demonstrates the basic testing capabilities of **DrMock**.

### Table of contents

* [Setup](#setup)<br/>
* [Source code](#source-code)<br/>
  + [Assert and compare](#assert-and-compare)<br/>
  + [Exceptions](#exceptions)<br/>
  + [Test tables](#test-tables)<br/>
  + [`USING_DRTEST`](#using_drtest)
* [Running the tests](#running-the-tests)<br/>
* [Caveats](#caveats)<br/>
  + [Commas in macro arguments](#commas-in-macro-arguments)<br/>
  + [Implicit conversion in test tables](#implicit-conversion-in-test-tables)
  + [Compile options, linking test executables and resource files](#compile-options-linking-test-executables-and-resource-files)
  + [Test names](#test-names)
  + [Row names](#row-names)
  + [Implicit conversions of number types](#implicit-conversions-of-number-types)

### Project structure

```
samples/basic
│   CMakeLists.txt
│   Makefile
│   basicTest.cpp
```

### Requirements

This project requires an installation of **DrMock** in `build/install/`
or a path container in the `CMAKE_PREFIX_PATH` variable. If your
installation of **DrMock** is located elsewhere, you must change the
value of `CMAKE_PREFIX_PATH`.

## Setup

Recall that **DrMock** requires you to configure your project with
CMake. The `CMakeLists.txt` is faily simple. To fullfil **DrMock**'s
requirement for C++17, do

```cmake
set(CMAKE_CXX_STANDARD 17)
```

Next, find the package `DrMock::DrMock`.

```cmake
find_package(DrMock)
```

This will fail if `DrMock::DrMock` cannot be found in the
`CMAKE_PREFIX_PATH`. Make sure that **DrMock** is installed there, set
the prefix in the `Makefile` (see below) or call `cmake` yourself with
the `-DCMAKE_PREFIX_PATH` directive.

Finding the **DrMock** package
automatically imports the macro `drmock_test`, which may be used to
register tests.

```cmake
enable_testing()
drmock_test(TESTS basicTest.cpp)
```

You could add further tests by providing addition arguments to the
macro, like so:

```cmake
drmock_test(TESTS
    test0.cpp
    test1.cpp
    # ...
)
```

Now lets take a look at the `Makefile`:

```makefile
default:
  mkdir -p build && cd build && cmake .. -DCMAKE_PREFIX_PATH=../../build/install
  cd build && make -j$(num_threads) && ctest --output-on-failure
```

Nothing fancy here. Note the ctest call after building.

If you haven't already, set the `CMAKE_PREFIX_PATH` environment variable
to the installation directory of the **DrMock** package. If you've done
`make && make install` in the **DrMock** source folder and haven't moved
`build/install`, the default `CMAKE_PREFIX_PATH` set in the `Makefile` is
referencing the correct directory.

## Source code

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

### Assert and compare

Use `DRTEST_ASSERT` to check if an expression is `true`:
```cpp
DRTEST_TEST(someTest)
{
  DRTEST_ASSERT(true);
  DRTEST_ASSERT(3 + 4 == 7);
}
```

To compare objects, use `DRTEST_ASSERT_[...]`:
```cpp
DRTEST_TEST(anotherTest)
{
  // 3 + 4 == 7
  DRTEST_ASSERT_EQ(3 + 4, 7);
  // 7 > 6
  DRTEST_ASSERT_GT(7, 6);
  // 8 + 5 >= 13
  DRTEST_ASSERT_GE(8 + 5, 13);
  // - 3 < 0
  DRTEST_ASSERT_LT(-3, 0);
  // 9 <= 10
  DRTEST_ASSERT_LE(9, 10);
}
```
These macros will print the left- and right-hand side of the comparison
in case of failure (if possible).

### Exceptions

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
    str = "bar";
    throw std::runtime_error{str},
    std::runtime_error
  );
```
Observe that the statements are seperated by semicolons. The last
statement of the first argument of `DRTEST_ASSERT_THROW` may or may not
end with a `;`.

To assert that certain tests fail, use `DRTEST_ASSERT_TEST_FAIL`:
```cpp
DRTEST_ASSERT_TEST_FAIL(
    DRTEST_ASSERT(2 + 2 == 5);
  );
```
This macro may also be applied to multiple statements at once, using the
same syntax as `DRTEST_ASSERT_THROW`.

**Note.**
```cpp
DRTEST_ASSERT_TEST_FAIL(throw std::runtime_error{"foo"});
```
will cause the test to fail.

### Test tables

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

As of version `0.5`, you can also use the utility function `addColumns`
to get the same result:

```cpp
DRTEST_DATA(someTestWithTable)
{
  drtest::addColumns<int, int, int, std::string>("lhs", "rhs", "expected", "randomStuff");

  // ...
}
```

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
DRTEST_FETCH(Type, column_name)
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

  DRTEST_ASSERT_EQ(lhs + rhs, expected);
  DRTEST_ASSERT(randomStuff.size() > 2);
}
```
The first of these tests will check `3 + 4 == 7`, `1593 + 2478 == 4071`
and `2 + 2 == 5`. In case of failure, each of these will be displayed as
individual tests.

### `USING_DRTEST`

If long macro names like `DRTEST_ASSERT_EQ` are impractical and you're
certain that no collisions will result from this, you can `#define
USING_DRTEST` to remove the `DRTEST_` prefix from all `DRTEST_*` macros.

The following macros are impacted:
`DRTEST_FETCH`,
`DRTEST_DATA`,
`DRTEST_TEST`,
`DRTEST_ASSERT`,
`DRTEST_ASSERT_EQ`,
`DRTEST_ASSERT_NE`,
`DRTEST_ASSERT_LE`,
`DRTEST_ASSERT_LT`,
`DRTEST_ASSERT_GE`,
`DRTEST_ASSERT_GT`,
`DRTEST_ASSERT_THROW`,
`DRTEST_ASSERT_TEST_FAIL`,
`DRTEST_VERIFY_MOCK`,
`DRTEST_ASSERT_ALMOST_EQUAL`.

Like other `using namespace ...` directives, please use this one with
caution!

## Running the tests

Do `make`. This should yield the following:

```
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
*FAIL  someTestWithTable, This test fails (117):
    (lhs + rhs)
      4
    (expected ==)
      5

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
```
TEST   someTestWithTable, This test fails
*FAIL  someTestWithTable, This test fails (117):
    (lhs + rhs)
      4
    (expected ==)
      5
```
**DrMock** prints the failed test's name (`someTestWithTable`), the row
of the failed subtest (`This test will fail`, if the test uses test
tables), and the offending line (`117`). Furthermore, as the test used
`DRTEST_ASSERT_EQ`, the objects that failed the comparison are printed,
along with their variable name.

Change the test table so that the test check if `2 + 2 == 4` instead of
`2 + 2 == 5` and run `make` again:

```
    Start 1: basicTest
1/1 Test #1: basicTest ........................   Passed    0.00 sec

100% tests passed, 0 tests failed out of 1

Total Test time (real) =   0.01 sec
```

## Tags

As of version `0.5`, **DrMock** offers `xfail` and `skip` tags for
tests. A row can be tagged by appending the tag to the `addRow` call, or
by calling `tagRow` seperately:

```cpp
DRTEST_DATA(someTestWithTable)
{
  // ...

  drtest::addRow(
      "This test will be skipped",
      2,
      2,
      4,
      std::string{"..."},
      drtest::tags::skip
    );
  drtest::addRow(
      "This test is expected to fail",
      -2,
      -2,
      4,
      std::string{"0>0"}
    );
  drtest::tagRow("This test is expected to fail", drtest::tags::xfail);
}
```

Add the above to the file and run `ctest` with `--verbose`. You will see
the following result:

```shell
test 1
    Start 1: basicTest

1: Test command: /Users/malte/drmock/samples/basic/build/basicTest
1: Test timeout computed to be: 10000000
1: TEST   someTest
1: PASS   someTest
1: TEST   anotherTest
1: PASS   anotherTest
1: TEST   exceptionTest
1: PASS   exceptionTest
1: TEST   someTestWithTable, Small numbers
1: PASS   someTestWithTable, Small numbers
1: TEST   someTestWithTable, Large numbers
1: PASS   someTestWithTable, Large numbers
1: TEST   someTestWithTable, This test fails
1: PASS   someTestWithTable, This test fails
1: TEST   someTestWithTable, This test will be skipped
1: SKIP   someTestWithTable, This test will be skipped
1: TEST   someTestWithTable, This test is expected to fail
1: XFAIL  someTestWithTable, This test is expected to fail (134): 
1:     (lhs + rhs) 
1:       -4
1:     (expected ==)
1:       4
1: 
1: ****************
1: ALL PASS
1/1 Test #1: basicTest ........................   Passed    0.00 sec

100% tests passed, 0 tests failed out of 1

Total Test time (real) =   0.01 sec
```

A test without a corresponding `DRTEST_DATA(...)` can also be skipped or
xfailed by calling `drtest::skip()` or `drtest::xfail()`. Note that the
placement of the call is important. Place `drtest::xfail` somewhere
before the call or assertion that is expected to fail and `drtest::skip`
at the point at which you want to skip to the next test:

```cpp
DRTEST_TEST(...)
{
  /* statements */;  // These will be executed and may cause failure.
  drtest::skip();
  // This will not be executed.
}
```

Note that `xfail` currently exists the test after the first failure:

```cpp
DRTEST_TEST(...)
{
  drtest::xfail();
  DRTEST_ASSERT_EQ(2 + 2, 5);
  /* Do something... */;  // Will not be executed!
}
```

## Floating point comparison

As of version `0.5`, **DrMock** offers a macro for floating point
comparison, whose "signature" is the following:

```cpp
template<typename T>
DRTEST_ASSERT_ALMOST_EQUAL(T actual, T expected)
```

We expect `T` to be `float`, `double` or `long double`. The macro
performs the following check:

```
|actual - expected| <= abs_tol + rel_tol*|expected|
```

The values `abs_tol` and `rel_tol` are called _absolute_ and _relative
tolerance_, resp. They are, by default, equal to `1e-06`, but may be set
for an entire test file by `#define`-ing the macros `DRTEST_ABS_TOL` and
`DRTEST_REL_TOL` resp., _before including_ the master header `Test.h`:

```cpp
#define DRTEST_ABS_TOL 1e-03
#define DRTEST_REL_TOL 0
#include "Test.h"
```

They may also be set for individual tests by using `drtest::abs_tol` and
`drtest::rel_tol` functions _inside the test_ (thus overriding the default
or the definition using the `#define` directive). For example:

```cpp
DRTEST_TEST(almost_equal_custom)
{
  DRTEST_ASSERT_ALMOST_EQUAL(0.000001f, 0.0f);  // Uses default/values set by #define.

  drtest::abs_tol(1.0);
  DRTEST_ASSERT_ALMOST_EQUAL(2.0 + 2.0, 5.0);

  // Note that this check is not a symmetric function in
  // `(actual, expected)`, unless `rel_tol` is zero.
  drtest::rel_tol(0.5);
  DRTEST_ASSERT_ALMOST_EQUAL(50.0, 100.0);
  DRTEST_ASSERT_TEST_FAIL(DRTEST_ASSERT_ALMOST_EQUAL(100.0, 50.0));
}
```

## Caveats

### Commas in macro arguments

The error
```
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
will throw this error. The macro will assume it is called with
`std::vector<int>{0` as first argument, `1` as second, etc.  To solve
this, use parens `()` to encapsulate these commas:
```cpp
DRTEST_ASSERT(
    (std::vector<int>{0, 1, 2})
    ==
    (std::vector<int>{3, 4, 5})
  );
```

### Implicit conversion in test tables

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

### Compile options, linking test executables and resource files

You may set the compile options of the test executables using the
`OPTIONS` parameter of `drmock_test`. If `OPTIONS` is left undefined, the
following default options are used: `-Wall`, `-Werror`, `-g`, `-fPIC`,
`-pedantic`, `-O0`. For example, to compile only with `-Wall` and
`-Werror`, do
```cmake
drmock_test(
    TESTS
        test.cpp
    OPTIONS
        -Wall
        -Werror
)
```

To link the test executables against any number of target library, use
the `LIBS` parameter (also described in the [next section](mock.md)).
For example, to link the test above against `pthread`, do
```cmake
drmock_test(
    TESTS
        test.cpp
    LIBS
        pthread
    OPTIONS
        -Wall
        -Werror
)
```

The `RESOURCE` parameter may be used to add source files to the test
executable:
```cmake
drmock_test(
    TESTS
        test.cpp
    RESOURCES
        symbols.cpp
)
```
Maybe `symbols.cpp` contains symbols required by a header included in
`test.cpp`.
Another common use-case is that of including `.qrc` files (Qt resource
files) to the executable if they are required by the test.

### Test names

Any `snake_case` or `camelCase` name may be used for a test. Avoid
any name containing `DRTEST` or `DRMOCK`, or `ALL_CAPS` in general.

By default, test functions are stored in the namespace `test::`. If this
is inconvenient for you, you can redefine the namespace by setting the
macro `DRTEST_NAMESPACE` _before including_ `Test.h`:

```cpp
#define DRTEST_NAMESPACE mytest
#include <DrMock/Test.h>
```

### Row names

Any `snake_case` or `camelCase` name may be used as row name, with the
exception of the emtpy string. Avoid any name containing `DRTEST` or
`DRMOCK`, or `ALL_CAPS` in general.

Furthermore, duplicate row names are not allowed.

### Implicit conversions of number types

Beware of implicit conversions when using the macros! For example, instead of
```cpp
DRTEST_ASSERT_ALMOST_EQUAL(0.9, 1)
```
use
```cpp
DRTEST_ASSERT_ALMOST_EQUAL(0.9, 1.0)
```

Otherwise, you might end up with an error similar to this one:

```shell
/Users/malte/drmock/tests/Test.cpp:347:3: error: no matching function for call to 'almostEqual'
  DRTEST_ASSERT_ALMOST_EQUAL(0.9, 1);
  ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/Users/malte/drmock/src/test/TestMacros.h:109:11: note: expanded from macro 'DRTEST_ASSERT_ALMOST_EQUAL'
  if (not drtest::almostEqual(actual, expected)) \
          ^~~~~~~~~~~~~~~~~~~
/Users/malte/drmock/src/test/Interface.tpp:46:1: note: candidate template ignored: deduced conflicting types for parameter 'T' ('double' vs. 'int')
almostEqual(T actual, T expected)
^
```
