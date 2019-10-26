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

```
set(CMAKE_CXX_STANDARD 17)
```

Next, find the package `DrMock::Core`.

```
find_package(DrMock COMPONENTS Core REQUIRED)
```

This will fail if `DrMock::Core` cannot be found in the
`CMAKE_PREFIX_PATH`. Make sure that DrMock installed there, or set the
prefix in the `Makefile` (see below). 

Finding the **DrMock** package
automatically import the macro `DrMockTest`, which may be used to
register tests.

```
enable_testing()
DrMockTest(TESTS
  basicTest.cpp
)
```

You could add further tests by providing addition arguments to the
macro, like so:

```
DrMockTest(TESTS
  test0.cpp
  test1.cpp
  # ...
)
```

Now lets take a look at the `Makefile`:

```
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
```
DRTEST_TEST(someTest)
{
  // ...
}
```
A source file name hold an arbitrary number of `DRTEST_TEST(...)` calls.

#### Assertion and comparison

Use `DRTEST_ASSERT` to check if an expression is `true`:
```
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
```
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
```
DRTEST_ASSERT_THROW(statement, exceptionType);
```
which executes `statement` and checks if an exception of type
`exceptionType` is thrown:

```
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
```
DRTEST_ASSERT_THROW(
    std::string str = "foo";
    throw std::runtime_error{str};
    (void)str,  // Mark `str` as used.
    std::runtime_error
  );
```
Observe that the statements are seperated by semicolons.

To check for any throw, use `DRTEST_ASSERT_FAIL`:
```
DRTEST_ASSERT_FAIL(throw std::runtime_error{"foo!"});
```

#### Test tables

Test data is organised in _test tables_.
To initialize a test table for the test `someTestWithTable`, use
```
DRTEST_DATA(someTestWithTable)
{
  // ...
}
```
Columns and rows can be pushed to the table by using
```
drtest::addColumn<Type>("column_name");
```
For instance,
```
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
```
drtest::addRows(
    "row description", 
    1stColumnEntry,
    2ndColumnEntry,
    ...
  );
```
For example:
```
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
```
DRTEST(Type, column_name)
```
Note the lack of double quote in `column_name`!

Thus, to get the test data from above, do
```
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
```
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
```
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

```
    Start 1: basicTest
1/1 Test #1: basicTest ........................   Passed    0.00 sec

100% tests passed, 0 tests failed out of 1

Total Test time (real) =   0.01 sec
```

### Caveats

#### Commas in macro arguments

The error
```
error: too many arguments provided to function-like macro invocation
```
can be caused by not properly isolating commas occuring in macro
arguments. For example,
```
DRTEST_ASSERT(
    std::vector<int>{0, 1, 2} 
    == 
    std::vector<int>{3, 4, 5}
  );
```
will throw this error. The macro will think it is called with the
arguments `std::vector<int>{0`, `1`, `2} == std::vector<int>{3`, etc...
To solve this, use parens `()` to encapsulate these commas:
```
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
```
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
