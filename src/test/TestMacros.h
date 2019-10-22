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

#ifndef DRMOCK_SRC_TEST_TESTMACROS_H
#define DRMOCK_SRC_TEST_TESTMACROS_H

#include "Global.h"
#include "TestFailure.h"
#include "FunctionInvoker.h"

#define DRTEST_FETCH(Type, name) \
Type name{drtest::detail::Singleton<drtest::detail::Global>::get()->fetchData<Type>(#name)}

#define DRTEST_DATA(name) \
void name##Data(); \
namespace drtest { namespace detail { \
FunctionInvoker name##_data_pusher{[] () { Singleton<Global>::get()->addDataFunc(#name, &name##Data); }}; \
}} \
void name##Data()

#define DRTEST_TEST(name) \
void name(); \
namespace drtest { namespace detail { \
FunctionInvoker name##_test_pusher{[] () { Singleton<Global>::get()->addTestFunc(#name, &name); }}; \
}} \
void name()

#define DRTEST_ASSERT(p) \
if (not (p)) throw drtest::detail::TestFailure{__LINE__, #p}

#define DRTEST_COMPARE(lhs, rhs) \
if (not (lhs == rhs)) throw drtest::detail::TestFailure{__LINE__, #lhs, #rhs, lhs, rhs};

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

#define DRTEST_ASSERT_FAIL(p) \
do { \
  try \
  { \
    p; \
  } \
  catch(const std::exception& e) \
  { \
    break; \
  } \
  throw drtest::detail::TestFailure{__LINE__, "expected failure: " #p}; \
} while (false)

#define DRTEST_VERIFY_MOCK(m) \
if (not m.verify()) throw drtest::detail::TestFailure{__LINE__, m.makeFormatedErrorString()}

#endif /* DRMOCK_SRC_TEST_TESTMACROS_H */
