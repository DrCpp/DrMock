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
#include "mock/FooMock.h"

using namespace drmock::samples;

DRTEST_TEST(voidNoExpect)
{
  auto foo = std::make_shared<FooMock>();

  foo->h();
  DRTEST_ASSERT(not foo->mock.control.verify());
}

DRTEST_TEST(voidExpect)
{
  auto foo = std::make_shared<FooMock>();

  foo->mock.h().push().expects();
  foo->h();
  DRTEST_ASSERT(foo->mock.control.verify());
}

DRTEST_TEST(missingReturn)
{
  auto foo = std::make_shared<FooMock>();
  float x;

  foo->mock.g().push().expects();
  x = foo->g();
  DRTEST_ASSERT(not foo->mock.control.verify());
  DRTEST_ASSERT_LE(x, 0.0001f);
}

DRTEST_TEST(timesRange)
{
  auto foo = std::make_shared<FooMock>();
  int x;

  foo->mock.f().push()
      .expects("foo", 123)
      .times(1, 3)
      .returns(5);

  // 0
  DRTEST_ASSERT(not foo->mock.control.verify());

  // 1
  x = foo->f("foo", 123);
  DRTEST_ASSERT(foo->mock.control.verify());
  DRTEST_ASSERT_EQ(x, 5);

  // 2
  x = foo->f("foo", 123);
  DRTEST_ASSERT(foo->mock.control.verify());
  DRTEST_ASSERT_EQ(x, 5);

  // 3
  x = foo->f("foo", 123);
  DRTEST_ASSERT(foo->mock.control.verify());
  DRTEST_ASSERT_EQ(x, 5);

  // 4
  x = foo->f("foo", 123);
  DRTEST_ASSERT(not foo->mock.control.verify());
  DRTEST_ASSERT_EQ(x, 0);
}

DRTEST_TEST(timesExact)
{
  auto foo = std::make_shared<FooMock>();
  int x;

  foo->mock.f().push()
      .expects("foo", 123)
      .times(2)
      .returns(5);

  // 0
  DRTEST_ASSERT(not foo->mock.control.verify());

  // 1
  x = foo->f("foo", 123);
  DRTEST_ASSERT(not foo->mock.control.verify());
  DRTEST_ASSERT_EQ(x, 5);

  // 2
  x = foo->f("foo", 123);
  DRTEST_ASSERT(foo->mock.control.verify());
  DRTEST_ASSERT_EQ(x, 5);

  // 3
  x = foo->f("foo", 123);
  DRTEST_ASSERT(not foo->mock.control.verify());
  DRTEST_ASSERT_EQ(x, 0);
}

DRTEST_TEST(enforceOrder)
{
  auto foo = std::make_shared<FooMock>();

  foo->mock.f().push()
      .expects("foo", 123)
      .times(1)
      .returns(1);
  foo->mock.f().push()
      .expects("bar", 456)
      .times(1)
      .returns(2);

  int x = foo->f("bar", 456);
  int y = foo->f("foo", 123);

  DRTEST_ASSERT(not foo->mock.control.verify());

  // Out of turn call results in default value return.
  DRTEST_ASSERT_EQ(x, 0);
  DRTEST_ASSERT_EQ(y, 1);
}

