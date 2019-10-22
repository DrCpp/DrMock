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

#include "test/Test.h"

#include <iostream>

DRTEST_TEST(initTestCase)
{
  DRTEST_LOG_INFO("initTestCase");
}

DRTEST_TEST(cleanupTestCase)
{
  DRTEST_LOG_INFO("cleanupTestCase");
}

DRTEST_TEST(init)
{
  DRTEST_LOG_INFO("init");
}

DRTEST_TEST(cleanup)
{
  DRTEST_LOG_INFO("cleanup");
}

/*DRTEST_DATA(dataWithoutTest)
{
  drtest::addColumn<std::string>("col1");
  drtest::addRow("row 1", std::string{"1"});
}

DRTEST_TEST(testWithoutData)
{
  DRTEST_ASSERT_FAIL(DRTEST_FETCH(std::string, col1));
}*/

DRTEST_TEST(test1)
{
  DRTEST_ASSERT_THROW(throw std::runtime_error{""}, std::runtime_error);
  DRTEST_ASSERT_FAIL(
      DRTEST_ASSERT_THROW(int i = 1; (void)i, std::runtime_error)
    );
  DRTEST_LOG_INFO(__PRETTY_FUNCTION__);
}

DRTEST_TEST(test2)
{
  DRTEST_LOG_INFO(__PRETTY_FUNCTION__);
  DRTEST_ASSERT_FAIL(DRTEST_ASSERT(false));
  DRTEST_ASSERT_FAIL(DRTEST_ASSERT_FAIL(DRTEST_ASSERT(true)));
}

DRTEST_DATA(test3)
{
  drtest::addColumn<std::string>("col1");
  drtest::addColumn<int>("col2");
  drtest::addColumn<double>("col3");

  drtest::addRow(
      "row 1",
      std::string{"1"},
      1,
      1.11
    );

  drtest::addRow(
      "row 2",
      std::string{"2"},
      2,
      2.22
    );

  drtest::addRow(
      "row 3",
      std::string{"3"},
      3,
      3.33
    );
}

DRTEST_TEST(test3)
{
  DRTEST_LOG_INFO(__PRETTY_FUNCTION__);
  DRTEST_FETCH(std::string, col1);
  DRTEST_FETCH(int, col2);
  DRTEST_FETCH(double, col3);

  if (col2 == 2)
  {
    DRTEST_ASSERT(col2 == 2);
  }
  else
  {
    DRTEST_ASSERT_FAIL(DRTEST_ASSERT(col2 == 2));
  }

  if (col2 == 1)
  {
    DRTEST_COMPARE(col2, 1);
  }
  else
  {
    DRTEST_ASSERT_FAIL(DRTEST_COMPARE(col2, 1));
  }

  (void) col3;
}

DRTEST_TEST(test4)
{
  DRTEST_LOG_INFO(__PRETTY_FUNCTION__);
  DRTEST_ASSERT_FAIL(throw std::runtime_error{"foo"});
}

DRTEST_DATA(test5)
{
  drtest::addColumn<int>("summand1");
  drtest::addColumn<int>("summand2");
  drtest::addColumn<int>("summe");

  drtest::addRow(
      "1",
      4,
      3,
      7
    );

  drtest::addRow(
      "2",
      5,
      1,
      6
    );

  drtest::addRow(
      "3",
      2,
      1,
      6
    );
}

DRTEST_TEST(test5)
{
  DRTEST_FETCH(int, summand1);
  DRTEST_FETCH(int, summand2);
  DRTEST_FETCH(int, summe);

  if (summand1 == 2)
  {
    DRTEST_ASSERT_FAIL(DRTEST_COMPARE(summand1 + summand2, summe));
  }
  else
  {
    DRTEST_COMPARE(summand1 + summand2, summe);
  }
}
