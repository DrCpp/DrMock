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

DRTEST_TEST(signalsAndSlots)
{
  auto foo1 = std::make_shared<FooMock>();
  auto foo2 = std::make_shared<FooMock>();

  // Instruct `foo2` to expect exactly one call of `someSlot` with
  // `"foo"` as argument.
  foo2->mock.someSlot().push()
      .expects("foo")
      .times(1);

  QObject::connect(
      foo1.get(), &IFoo::someSignal,
      foo2.get(), &IFoo::someSlot
    );

  emit foo1->someSignal("foo");

  DRTEST_ASSERT(foo2->mock.verify());
}
