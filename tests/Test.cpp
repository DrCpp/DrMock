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

#ifdef _MSC_VER
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif /* _MSC_VER */

#include <iostream>

#define USING_DRTEST
#include <DrMock/Test.h>

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

DRTEST_TEST(assert_throw)
{
  DRTEST_LOG_INFO(__PRETTY_FUNCTION__);
  DRTEST_ASSERT_THROW(throw std::runtime_error{""}, std::runtime_error);
  DRTEST_ASSERT_TEST_FAIL(
      DRTEST_ASSERT_THROW(int i = 1; (void)i, std::runtime_error)
    );
}

DRTEST_TEST(assert_test_fail)
{
  DRTEST_LOG_INFO(__PRETTY_FUNCTION__);
  DRTEST_ASSERT_TEST_FAIL(DRTEST_ASSERT(false));
  DRTEST_ASSERT_TEST_FAIL(DRTEST_ASSERT_TEST_FAIL(DRTEST_ASSERT(true)));
}

DRTEST_TEST(assert_operator)
{
  DRTEST_ASSERT_EQ(10, 10);
  DRTEST_ASSERT_TEST_FAIL(DRTEST_ASSERT_EQ(10, 11));

  DRTEST_COMPARE(10, 10);
  DRTEST_ASSERT_TEST_FAIL(DRTEST_COMPARE(10, 11));

  DRTEST_ASSERT_NE(10, 11);
  DRTEST_ASSERT_TEST_FAIL(DRTEST_ASSERT_NE(10, 10));

  DRTEST_ASSERT_LE(10, 11);
  DRTEST_ASSERT_LE(10, 10);
  DRTEST_ASSERT_TEST_FAIL(DRTEST_ASSERT_LE(10, 9));

  DRTEST_ASSERT_LT(10, 11);
  DRTEST_ASSERT_TEST_FAIL(DRTEST_ASSERT_LT(10, 10));
  DRTEST_ASSERT_TEST_FAIL(DRTEST_ASSERT_LT(10, 9));

  DRTEST_ASSERT_GE(11, 10);
  DRTEST_ASSERT_GE(10, 10);
  DRTEST_ASSERT_TEST_FAIL(DRTEST_ASSERT_GE(9, 10));

  DRTEST_ASSERT_GT(11, 10);
  DRTEST_ASSERT_TEST_FAIL(DRTEST_ASSERT_GT(10, 10));
  DRTEST_ASSERT_TEST_FAIL(DRTEST_ASSERT_GT(9, 10));
}

DRTEST_DATA(test_with_data_1)
{
  drtest::addColumns<std::string, int, double>("col1", "col2", "col3");

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

DRTEST_TEST(test_with_data_1)
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
    DRTEST_ASSERT_TEST_FAIL(DRTEST_ASSERT(col2 == 2));
  }

  if (col2 == 1)
  {
    DRTEST_COMPARE(col2, 1);
  }
  else
  {
    DRTEST_ASSERT_TEST_FAIL(DRTEST_COMPARE(col2, 1));
  }

  (void) col3;
}

DRTEST_DATA(test_with_data_2)
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

DRTEST_TEST(test_with_data_2)
{
  DRTEST_FETCH(int, summand1);
  DRTEST_FETCH(int, summand2);
  DRTEST_FETCH(int, summe);

  if (summand1 == 2)
  {
    DRTEST_ASSERT_TEST_FAIL(DRTEST_COMPARE(summand1 + summand2, summe));
  }
  else
  {
    DRTEST_COMPARE(summand1 + summand2, summe);
  }
}

DRTEST_TEST(test_without_data)
{
  DRTEST_ASSERT_THROW(DRTEST_FETCH(std::string, col1), std::logic_error);
}

struct A
{
  bool operator==(const A&) const
  {
    return true;
  }
};

DRTEST_TEST(streamIfStreamable)
{
  // The line below checks if DRTEST_ASSERT_EQ compiles despite class A
  // not having a streaming operator.
  DRTEST_ASSERT_EQ(A{}, A{});
}

#if defined(__unix__) || defined(__APPLE__)

DRTEST_TEST(death_success)
{
  DRTEST_ASSERT_DEATH(raise(SIGSEGV), SIGSEGV);
  DRTEST_ASSERT_DEATH(raise(SIGCHLD), SIGCHLD);
  DRTEST_ASSERT_DEATH(raise(SIGABRT), SIGABRT);
  DRTEST_ASSERT_DEATH(volatile int* foo = nullptr; *foo =123, SIGSEGV);
}

DRTEST_TEST(death_failure_no_raise)
{
  DRTEST_ASSERT_TEST_FAIL(DRTEST_ASSERT_DEATH(int x = 0; (void)x, SIGSEGV));
}

DRTEST_TEST(death_failure_wrong_raise)
{
  DRTEST_ASSERT_TEST_FAIL(DRTEST_ASSERT_DEATH(raise(SIGXFSZ), SIGSEGV));
}

#endif /* defined(__unix__) || defined(__APPLE__) */

// Test that a test may be called the same an a drtest interface
// function; see issue #7 for details.
DRTEST_TEST(addRow)
{
  DRTEST_ASSERT(true);
}

DRTEST_DATA(almost_equal_default_ok)
{
  drtest::addColumn<float>("actual");
  drtest::addColumn<float>("expected");
  drtest::addRow("almost zero", 0.000001f, 0.0f);
  drtest::addRow("small", 0.999999f, 1.0f);
  drtest::addRow("large", 999'999.0f, 1'000'000.0f);
}

DRTEST_TEST(almost_equal_default_ok)
{
  DRTEST_FETCH(float, actual);
  DRTEST_FETCH(float, expected);
  DRTEST_ASSERT_ALMOST_EQUAL(actual, expected);
}

DRTEST_DATA(almost_equal_default_fail)
{
  drtest::addColumn<float>("actual");
  drtest::addColumn<float>("expected");
  drtest::addRow("almost zero", 0.0001f, 0.0f);
  drtest::addRow("small", 0.9999f, 1.0f);
  drtest::addRow("large", 9'999.0f, 10'000.0f);
}

DRTEST_TEST(almost_equal_default_fail)
{
  DRTEST_FETCH(float, actual);
  DRTEST_FETCH(float, expected);
  DRTEST_ASSERT_TEST_FAIL(DRTEST_ASSERT_ALMOST_EQUAL(actual, expected));
}

DRTEST_TEST(almost_equal_custom)
{
  drtest::abs_tol(1.0);
  DRTEST_ASSERT_ALMOST_EQUAL(2.0 + 2.0, 5.0);

  drtest::rel_tol(1.0);
  DRTEST_ASSERT_ALMOST_EQUAL(27, 50);

  drtest::abs_tol(0.0);
  drtest::rel_tol(0.0);
  DRTEST_ASSERT_ALMOST_EQUAL(1.0, 1.0);

  drtest::rel_tol(0.5);
  DRTEST_ASSERT_ALMOST_EQUAL(50.0, 100.0);
  DRTEST_ASSERT_TEST_FAIL(DRTEST_ASSERT_ALMOST_EQUAL(100.0, 50.0));
}

DRTEST_DATA(tags)
{
  drtest::addColumn<int>("lhs");
  drtest::addColumn<int>("rhs");
  drtest::addColumn<int>("expected");

  drtest::addRow("row 1", 1, 1, 2);
  drtest::addRow("row 2", 2, 2, 5, drtest::tags::skip);
  drtest::addRow("row 3", 4, 4, 9, drtest::tags::xfail);
}

DRTEST_TEST(tags)
{
  DRTEST_FETCH(int, lhs);
  DRTEST_FETCH(int, rhs);
  DRTEST_FETCH(int, expected);
  auto sum = lhs + rhs;
  DRTEST_ASSERT_EQ(sum, expected);  // This will raise if row 2 or 3 are not skipped!
}

DRTEST_DATA(tagRow)
{
  drtest::addColumn<int>("lhs");
  drtest::addColumn<int>("rhs");
  drtest::addColumn<int>("expected");

  drtest::addRow("row 1", 1, 1, 2);
  drtest::addRow("row 2", 2, 2, 5);
  drtest::addRow("row 3", 4, 4, 9, drtest::tags::skip);
  drtest::tagRow("row 2", drtest::tags::skip);
  drtest::tagRow("row 3", drtest::tags::xfail);
}

DRTEST_TEST(tagRow)
{
  DRTEST_FETCH(int, lhs);
  DRTEST_FETCH(int, rhs);
  DRTEST_FETCH(int, expected);
  auto sum = lhs + rhs;
  DRTEST_ASSERT_EQ(sum, expected);  // This will raise if row 2 or 3 are not skipped!
}

DRTEST_TEST(skip)
{
  drtest::skip();
  throw std::logic_error{"this should never happen..."};
}

DRTEST_TEST(xfail)
{
  drtest::xfail();
  DRTEST_ASSERT_EQ(2 + 2, 5);
}

DRTEST_TEST(rowCollision)
{
  drtest::detail::TestObject test{"test"};
  test.addColumn<int>("col");
  test.addRow<int>("row", 1);
  DRTEST_ASSERT_THROW(test.addRow<int>("row", 2), std::logic_error);
}

DRTEST_TEST(emptyRowName)
{
  drtest::detail::TestObject test{"test"};
  test.addColumn<int>("col");
  DRTEST_ASSERT_THROW(test.addRow<int>("", 1), std::logic_error);
}

DATA(usingMacro)
{
  drtest::addColumns<int, int, int>("lhs", "rhs", "expected");
  drtest::addRow("row 1", 2, 2, 4);
}

TEST(usingMacro)
{
  FETCH(int, lhs);
  FETCH(int, rhs);
  FETCH(int, expected);
  ASSERT(true);
  ASSERT_EQ(lhs + rhs, expected);
  ASSERT_LE(4, 5);
  ASSERT_LT(4, 5);
  ASSERT_GE(5, 4);
  ASSERT_GT(5, 4);
  ASSERT_THROW(throw std::runtime_error{""};, std::runtime_error);
  ASSERT_TEST_FAIL(ASSERT_EQ(false, true));
  ASSERT_ALMOST_EQUAL(0.999999, 1.0);
}
