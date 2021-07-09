/* Copyright 2021 Ole Kliemann, Malte Kliemann
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
#include "mock/NoDefaultCtorMock.h"

using namespace outer::inner;

DRTEST_TEST(fails)
{
  {
    NoDefaultCtorMock mock{3};
    DRTEST_ASSERT(mock.mock.verify());
  }

  {
    NoDefaultCtorMock mock{3};
    DRTEST_ASSERT(mock.mock.f().verify());
    mock.f();
    DRTEST_ASSERT(not mock.mock.verify());
    DRTEST_ASSERT(not mock.mock.f().verify());
  }
}

DRTEST_TEST(succeeds)
{
  NoDefaultCtorMock mock{3};
  DRTEST_ASSERT(mock.mock.verify());

  mock.mock.f().push()
      .expects()
      .times(1);
  mock.f();
  DRTEST_ASSERT(mock.mock.verify());
  DRTEST_ASSERT(mock.mock.f().verify());
}
