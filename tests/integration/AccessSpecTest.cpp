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
#include "mock/AccessSpecMock.h"

using namespace outer::inner;

DRTEST_TEST(failsPublic)
{
  AccessSpecMock mock{};
  mock.publicFunc();
  DRTEST_ASSERT(not mock.mock.publicFunc().verify());
}

DRTEST_TEST(failsProtected)
{
  AccessSpecMock mock{};
  mock.protectedFunc();
  DRTEST_ASSERT(not mock.mock.protectedFunc().verify());
}

DRTEST_TEST(failsPrivate)
{
  AccessSpecMock mock{};
  mock.privateFunc();
  DRTEST_ASSERT(not mock.mock.privateFunc().verify());
}

DRTEST_TEST(successPublic)
{
  AccessSpecMock mock{};
  mock.mock.publicFunc().push().expects();
  mock.publicFunc();
  DRTEST_ASSERT(mock.mock.publicFunc().verify());
}

DRTEST_TEST(successProtected)
{
  AccessSpecMock mock{};
  mock.mock.protectedFunc().push().expects();
  mock.protectedFunc();
  DRTEST_ASSERT(mock.mock.protectedFunc().verify());
}

DRTEST_TEST(successPrivate)
{
  AccessSpecMock mock{};
  mock.mock.privateFunc().push().expects();
  mock.privateFunc();
  DRTEST_ASSERT(mock.mock.privateFunc().verify());
}
