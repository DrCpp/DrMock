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

// This test verifies that mixing Qt and non-Qt mocks doesn't cause
// compiler errors and doesn't change functionality.

#include <DrMock/Test.h>
#include "mock/NotAQtObjectMock.h"
#include "mock/SignalsMock.h"

using namespace outer::inner;

DRTEST_TEST(callSlot)
{
  {
    SignalsMock mock{};
    DRTEST_ASSERT(mock.mock.f().verify());
    mock.f();
    DRTEST_ASSERT(not mock.mock.control.verify());
    DRTEST_ASSERT(not mock.mock.f().verify());
  }

  {
    NotAQtObjectMock mock{};
    DRTEST_ASSERT(mock.mock.foo().verify());
    mock.foo();
    DRTEST_ASSERT(not mock.mock.control.verify());
    DRTEST_ASSERT(not mock.mock.foo().verify());
  }
}

DRTEST_TEST(succeeds)
{
  {
    SignalsMock mock{};
    DRTEST_ASSERT(mock.mock.control.verify());

    mock.mock.f().push()
        .expects()
        .times(1);
    mock.f();
    DRTEST_ASSERT(mock.mock.control.verify());
    DRTEST_ASSERT(mock.mock.f().verify());
  }

  {
    NotAQtObjectMock mock{};
    DRTEST_ASSERT(mock.mock.control.verify());

    mock.mock.foo().push()
        .expects()
        .times(1);
    mock.foo();
    DRTEST_ASSERT(mock.mock.control.verify());
    DRTEST_ASSERT(mock.mock.foo().verify());
  }
}
