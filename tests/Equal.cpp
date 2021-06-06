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
#include "mock/Equal.h"

using namespace drmock;

DRTEST_TEST(wrap_in_equal)
{
  auto eq = wrap_in_shared_equal<int>(123);
  DRTEST_ASSERT(eq->invoke(123));
  DRTEST_ASSERT(not eq->invoke(234));
  auto eq2 = wrap_in_shared_equal<int>(eq);
  DRTEST_ASSERT(eq2->invoke(123));
  DRTEST_ASSERT(not eq2->invoke(234));
}
