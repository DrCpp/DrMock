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

#ifndef DRMOCK_SRC_DRMOCK_TEST_TESTMACROS_H
#define DRMOCK_SRC_DRMOCK_TEST_TESTMACROS_H

#include <DrMock/test/Death.h>
#include <DrMock/test/FunctionInvoker.h>
#include <DrMock/test/Global.h>
#include <DrMock/test/TestFailure.h>

#ifndef DRTEST_NAMESPACE
#define DRTEST_NAMESPACE test
#endif

#define DRTEST_FETCH(Type, name) \
Type name{drutility::Singleton<drtest::detail::Global>::get()->fetchData<Type>(#name)}

#define DRTEST_DATA(name) \
void name##DRTEST_Data(); \
namespace DRTEST_NAMESPACE { \
drtest::detail::FunctionInvoker name##_data_pusher{[] () { drutility::Singleton<drtest::detail::Global>::get()->addDataFunc(#name, &name##DRTEST_Data); }}; \
} \
void name##DRTEST_Data()

#define DRTEST_TEST(name) \
namespace DRTEST_NAMESPACE { \
void name(); \
} \
namespace drtest { namespace detail { \
FunctionInvoker name##_test_pusher{[] () { drutility::Singleton<Global>::get()->addTestFunc(#name, &DRTEST_NAMESPACE:: name); }}; \
}} \
void DRTEST_NAMESPACE:: name()

#define DRTEST_ASSERT(p) \
do { if (not (p)) throw drtest::detail::TestFailure{__LINE__, #p}; } while (false)

#define DRTEST_ASSERT_EQ(lhs, rhs) \
do { if (not (lhs == rhs)) throw drtest::detail::TestFailure{__LINE__, "==", #lhs, #rhs, lhs, rhs}; } while (false)

#define DRTEST_COMPARE(lhs, rhs) DRTEST_ASSERT_EQ(lhs, rhs)

#define DRTEST_ASSERT_NE(lhs, rhs) \
do { if (not (lhs != rhs)) throw drtest::detail::TestFailure{__LINE__, "!=", #lhs, #rhs, lhs, rhs}; } while (false)

#define DRTEST_ASSERT_LE(lhs, rhs) \
do { if (not (lhs <= rhs)) throw drtest::detail::TestFailure{__LINE__, "<=", #lhs, #rhs, lhs, rhs}; } while (false)

#define DRTEST_ASSERT_LT(lhs, rhs) \
do { if (not (lhs < rhs)) throw drtest::detail::TestFailure{__LINE__, "<", #lhs, #rhs, lhs, rhs}; } while (false)

#define DRTEST_ASSERT_GE(lhs, rhs) \
do { if (not (lhs >= rhs)) throw drtest::detail::TestFailure{__LINE__, ">=", #lhs, #rhs, lhs, rhs}; } while (false)

#define DRTEST_ASSERT_GT(lhs, rhs) \
do { if (not (lhs > rhs)) throw drtest::detail::TestFailure{__LINE__, ">", #lhs, #rhs, lhs, rhs}; } while (false)

#define DRTEST_ASSERT_THROW(p, E) \
do { \
  try \
  { \
    p; \
  } \
  catch(const E&) \
  { \
    break; \
  } \
  throw drtest::detail::TestFailure{__LINE__, "no exception thrown: " #p}; \
} while (false)

#define DRTEST_ASSERT_TEST_FAIL(p) \
do { \
  try \
  { \
    p; \
  } \
  catch(const drtest::detail::TestFailure& e) \
  { \
    break; \
  } \
  throw drtest::detail::TestFailure{__LINE__, "expected failure: " #p}; \
} while (false)

#define DRTEST_VERIFY_MOCK(m) \
do { \
  if (not m.verify()) \
  { \
    throw drtest::detail::TestFailure{__LINE__, m.makeFormattedErrorString()}; \
  } \
} while (false)

#define DRTEST_ASSERT_ALMOST_EQUAL(actual, expected) \
do { \
  if (not drtest::almostEqual(actual, expected)) \
  { \
    throw drtest::detail::TestFailure{__LINE__, "~=", #actual, #expected, actual, expected}; \
  } \
} while (false)

#ifdef USING_DRTEST
#define FETCH DRTEST_FETCH
#define DATA DRTEST_DATA
#define TEST DRTEST_TEST
#define ASSERT DRTEST_ASSERT
#define ASSERT_EQ DRTEST_ASSERT_EQ
#define ASSERT_NE DRTEST_ASSERT_NE
#define ASSERT_LE DRTEST_ASSERT_LE
#define ASSERT_LT DRTEST_ASSERT_LT
#define ASSERT_GE DRTEST_ASSERT_GE
#define ASSERT_GT DRTEST_ASSERT_GT
#define ASSERT_THROW DRTEST_ASSERT_THROW
#define ASSERT_TEST_FAIL DRTEST_ASSERT_TEST_FAIL
#define VERIFY_MOCK DRTEST_VERIFY_MOCK
#define ASSERT_ALMOST_EQUAL DRTEST_ASSERT_ALMOST_EQUAL
#endif

#endif /* DRMOCK_SRC_DRMOCK_TEST_TESTMACROS_H */
