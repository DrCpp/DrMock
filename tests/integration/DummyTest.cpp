/* Copyright 2020 Ole Kliemann, Malte Kliemann
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

#include <DrMock/Test.h>
#include "mock/DummyMock.h"

using namespace outer::inner;

DRTEST_TEST(fails)
{
  DummyMock mock{};
  bool verified = mock.mock.f().verify();
  DRTEST_ASSERT(verified);
  Foo foo{};
  mock.f(foo);
  DRTEST_ASSERT(not mock.mock.control.verify());
  verified = mock.mock.f().verify();
  DRTEST_ASSERT(not verified);
}

DRTEST_TEST(success)
{
  DummyMock mock{};
  Foo foo1{};
  Foo foo2{};
  mock.mock.f().push()
      .expects(foo1)
      .times(1);
  mock.f(foo2);  // Trivial operator== must always return `true`.
  DRTEST_ASSERT(mock.mock.control.verify());
  DRTEST_ASSERT(mock.mock.f().verify());
}
