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
* [Running the tests](#running-the-tests)<br/>
* [Caveats](#caveats)<br/>
  + [Commas in macro arguments](#commas-in-macro-arguments)<br/>
  + [Implicit conversion in test tables](#implicit-conversion-in-test-tables)
  + [Compile options, linking test executables and resource files](#compile-options-linking-test-executables-and-resource-files)

### Project structure

```
samples/basic
│   CMakeLists.txt
│   Makefile
│   basicTest.cpp
```

### Requirements

This project requires an installation of **DrMock** in `prefix/` or the
`CMAKE_PREFIX_PATH` environment variable. If your installation of
**DrMock** is located elsewhere, you must change the value of
`CMAKE_PREFIX_PATH`.

## Setup

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
  mkdir -p build && cd build && cmake .. -DCMAKE_PREFIX_PATH=../../prefix
  cd build && make -j$(num_threads) && ctest --output-on-failure
```

Nothing fancy here. Note the ctest call after building.

If you haven't already, set the `CMAKE_PREFIX_PATH` environment variable
to the installation directory of the **DrMock** package. If you've done
`make && make install` in the **DrMock** source folder and haven't moved
`prefix`, the default `CMAKE_PREFIX_PATH` set in the `Makefile` is
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

To compare objects of some type `T`, the following `DRTEST_ASSERT_[...]`
macros may be used if `T` implements
`std::ostream& operator<<(ostream& os, const T&)`,
as done in the next test.
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
in case of failure.
If the compared type `T` doesn't provide a streaming operator, use
`DRTEST_ASSERT(lhs > rhs)`, etc. instead.

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
    throw std::runtime_error{str};
    (void)str,  // Mark `str` as used.
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
```
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

  DRTEST_ASSERT_EQ(lhs + rhs, expected);
  DRTEST_ASSERT(randomStuff.size() > 2);
}
```
The first of these tests will check `3 + 4 == 7`, `1593 + 2478 == 4071`
and `2 + 2 == 5`. In case of failure, each of these will be displayed as
individual tests.

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
`OPTIONS` parameter of `DrMockTest`. If `OPTIONS` is left undefined, the
following default options are used: `-Wall`, `-Werror`, `-g`, `-fPIC`,
`-pedantic`, `-O0`. For example, to compile only with `-Wall` and
`-Werror`, do
```cmake
DrMockTest(
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
DrMockTest(
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
DrMockTest(
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
