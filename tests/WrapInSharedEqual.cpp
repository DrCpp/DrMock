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

#include "test/Test.h"
#include "mock/detail/InvokeOnPack.h"
#include "mock/detail/WrapInSharedEqual.h"

using namespace drmock::detail;

DRTEST_TEST(wrap_in_equal)
{
  WrapInSharedEqual<std::tuple<int>> wrapper{};
  auto eq = wrapper.wrap(123);
  auto eq2 = wrapper.wrap(std::get<0>(eq));
  auto invoke_on_pack = InvokeOnPack<std::tuple<int>>{};
  DRTEST_ASSERT(invoke_on_pack(eq, 123));
  DRTEST_ASSERT(invoke_on_pack(eq2, 123));
  DRTEST_ASSERT(not invoke_on_pack(eq, 234));
  DRTEST_ASSERT(not invoke_on_pack(eq2, 234));
}
