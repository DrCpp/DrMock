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

#include <drmock/test/Test.h>
#include "mock/IncludeMock.h"

using namespace outer::inner;

DRTEST_TEST(include)
{
  // Check if the dummy classes are properly included.
  (void)SubDirBar{};
  (void)SubDirIncBar{};
}

DRTEST_TEST(fails)
{
  {
    IncludeMock mock{};
    DRTEST_ASSERT(mock.mock.verify());
  }

  {
    IncludeMock mock{};
    DRTEST_ASSERT(mock.mock.f().verify());
    mock.f();
    DRTEST_ASSERT(not mock.mock.verify());
    DRTEST_ASSERT(not mock.mock.f().verify());
  }
}

DRTEST_TEST(succeeds)
{
  IncludeMock mock{};
  DRTEST_ASSERT(mock.mock.verify());

  mock.mock.f().push()
      .expects()
      .times(1);
  mock.f();
  DRTEST_ASSERT(mock.mock.verify());
  DRTEST_ASSERT(mock.mock.f().verify());
}
