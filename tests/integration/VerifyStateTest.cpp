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

#include <DrMock/Test.h>
#include "mock/VerifyStateMock.h"

using namespace outer::inner;

DRTEST_TEST(singleSlot)
{
  VerifyStateMock foo{};
  foo.mock.set1().state()
      .transition("*", "on", true)
      .transition("*", "off", false);
  foo.mock.set2().state()
      .transition("", "*", "on", true)
      .transition("", "*", "off", false);
  // Make some random calls.
  foo.set1(false);
  foo.set2(false);
  foo.set1(true);
  foo.set2(false);
  DRTEST_ASSERT(foo.mock.control.verifyState("off"));
}

DRTEST_TEST(multipleSlots)
{
  VerifyStateMock foo{};
  foo.mock.set1().state()
      .transition("slot1", "*", "on", true)
      .transition("slot1", "*", "off", false);
  foo.mock.set2().state()
      .transition("slot2", "*", "on", true)
      .transition("slot2", "*", "off", false);
  // Make some random calls.
  foo.set1(false);
  foo.set2(true);
  DRTEST_ASSERT(foo.mock.control.verifyState("slot1", "off"));
  DRTEST_ASSERT(foo.mock.control.verifyState("slot2", "on"));
}
