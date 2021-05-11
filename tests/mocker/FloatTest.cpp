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
#include "mock/FloatMock.h"

using namespace outer::inner;

DRTEST_TEST(failureFloat)
{
  FloatMock mock{};
  mock.mock.f().push()
      .expects(drtest::almostEqual(1.0))
      .times(1);
  mock.f(0.9999f);
  DRTEST_ASSERT(mock.mock.verify());
  DRTEST_ASSERT(mock.mock.f().verify());
}

DRTEST_TEST(failureDouble)
{
  FloatMock mock{};
  mock.mock.f().push()
      .expects(drtest::almostEqual(1.0))
      .times(1);
  mock.f(0.9999);
  DRTEST_ASSERT(mock.mock.verify());
  DRTEST_ASSERT(mock.mock.f().verify());
}

DRTEST_TEST(successFloat)
{
  FloatMock mock{};
  mock.mock.f().push()
      .expects(drtest::almostEqual(1.0f))
      .times(1);
  mock.f(0.999999f);
  DRTEST_ASSERT(mock.mock.verify());
  DRTEST_ASSERT(mock.mock.f().verify());
}

DRTEST_TEST(successDouble)
{
  FloatMock mock{};
  mock.mock.f().push()
      .expects(drtest::almostEqual(1.0))
      .times(1);
  mock.f(0.999999);
  DRTEST_ASSERT(mock.mock.verify());
  DRTEST_ASSERT(mock.mock.f().verify());
}
