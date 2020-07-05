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
  auto foo = std::make_shared<FooMock>();
  auto bar = std::make_shared<FooMock>();

  // Instruct `bar` to expect exactly one call of `theSlot` with
  // `"foo"` as argument.
  bar->mock.theSlot().push()
      .expects("foo")
      .times(1);

  QObject::connect(
      foo.get(), &IFoo::theSignal,
      bar.get(), &IFoo::theSlot
    );

  // Instruct `foo` to emit `theSignal` with argument `"foo"` when
  // `theSlot` is called with `"bar"`.
  foo->mock.theSlot().push()
      .emits<const std::string&>(&IFoo::theSignal, "foo")
      .expects("bar")
      .times(1);  // Optional.
  foo->theSlot("bar");

  DRTEST_ASSERT(bar->mock.verify());
}
