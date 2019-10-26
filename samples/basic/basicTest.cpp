/* Copyright 2019 Ole Kliemann, Malte Kliemann
 *
 * This file is part of DrMock.
 *
 * DrMock is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * DrMock is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with DrMock.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "DrMock/Test.h"

// Initiate test `someTest`.
DRTEST_TEST(someTest)
{
  // Assert the following is true.
  DRTEST_ASSERT(true);
  DRTEST_ASSERT(3 + 4 == 7);
}

// Note: `DRTEST_COMPARE` requires an 
// ```
// std::ostream& operator<<(ostream& os, const T&)
// ```
// in order to use `DRTEST_COMPARE`. Use `DRMOCK_ASSERT(x == y)` if the
// class admits no such operator.
DRTEST_TEST(anotherTest)
{
  DRTEST_COMPARE(3 + 4, 7);
}

DRTEST_TEST(exceptionTest)
{
  // Check if the statement given in the first argument raises the
  // exception given in the second argument.
  DRTEST_ASSERT_THROW(
      throw std::runtime_error{"foo"},
      std::runtime_error
    );
  
  // If you wish, you can check multiple statements if any of them
  // raises.
  DRTEST_ASSERT_THROW(
      std::string str = "foo";
      (void)str;  // Mark `str` as used.
      throw std::runtime_error{str},
      std::runtime_error
    );

  // If you're not particular about which exception is thrown, you can
  // just use the following macro, which catches any exception derived
  // from `std::exception`.
  DRTEST_ASSERT_FAIL(throw std::runtime_error{"foo"});
}

DRTEST_DATA(someTestWithTable)
{
  // Define some columns (provide a unique name for each column).
  drtest::addColumn<int>("lhs");
  drtest::addColumn<int>("rhs");
  drtest::addColumn<int>("expected");
  drtest::addColumn<std::string>("randomStuff");
  
  // Populate the table with data.
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
      "This test fails",
      2,
      2,
      5,
      std::string{"baz"}
    );
}

DRTEST_TEST(someTestWithTable)
{
  // The following will be executed once for each row.
  
  // Fetch data from the columns. `lhs`, `rhs`, `expected` and
  // `randomStuff` are now variables, their values determined by the
  // current row.
  DRTEST_FETCH(int, lhs);
  DRTEST_FETCH(int, rhs);
  DRTEST_FETCH(int, expected);
  DRTEST_FETCH(std::string, randomStuff);

  // Run some tests with the fetched variables.
  DRTEST_COMPARE(lhs + rhs, expected);
  DRTEST_ASSERT(randomStuff.size() > 2);
}
