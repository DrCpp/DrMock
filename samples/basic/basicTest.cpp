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

// Note: `DRTEST_ASSERT_EQ`, etc. require an 
// ```
// std::ostream& operator<<(ostream& os, const T&)
// ```
// Use `DRMOCK_ASSERT(x == y)` if the class admits no such operator.
DRTEST_TEST(anotherTest)
{
  // 3 + 4 == 7
  DRTEST_ASSERT_EQ(3 + 4, 7);
  // 7 > 6
  DRTEST_ASSERT_GT(7, 6);
  // 8 + 5 >= 13
  DRTEST_ASSERT_GE(8 + 5, 13);
  // -3 < 0
  DRTEST_ASSERT_LT(-3, 0);
  // 9 <= 10
  DRTEST_ASSERT_LE(9, 10);
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

  // Assert the following tests to fail.
  DRTEST_ASSERT_TEST_FAIL(
      DRTEST_ASSERT(2 + 2 == 5)
    );
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
  DRTEST_ASSERT_EQ(lhs + rhs, expected);
  DRTEST_ASSERT(randomStuff.size() > 2);
}
