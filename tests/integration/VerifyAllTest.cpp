/* Copyright 2020 Ole Kliemann, Malte Kliemann
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
#include "mock/VerifyAllMock.h"

using namespace outer::inner;

DRTEST_TEST(fails)
{
  VerifyAllMock foo{};
  foo.mock.f().push().expects(1).times(1);
  foo.mock.g().push().expects(1.23f, 4.56).times(1);

  // Call only `f`.
  foo.f(1);

  // Should fail as `g` was not called as expected.
  DRTEST_ASSERT_TEST_FAIL(DRTEST_VERIFY_MOCK(foo.mock));
}

DRTEST_TEST(succeeds)
{
  VerifyAllMock foo{};
  foo.mock.f().push().expects(1).times(1);
  foo.mock.g().push().expects(1.23f, 4.56).times(1);

  // Call both methods.
  foo.f(1);
  foo.g(1.23f, 4.56);

  DRTEST_VERIFY_MOCK(foo.mock);
}
