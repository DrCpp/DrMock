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
#include "mock/VoidFuncOverloadMock.h"

using namespace outer::inner;

DRTEST_TEST(fails)
{
  {
    VoidFuncOverloadMock mock{};
    bool verified = mock.mock.func<>().verify();
    DRTEST_ASSERT(verified);
    mock.func();
    DRTEST_ASSERT(not mock.mock.verify());
    verified = mock.mock.func<>().verify();
    DRTEST_ASSERT(not verified);
  }

  {
    VoidFuncOverloadMock mock{};
    bool verified = mock.mock.func<int>().verify();
    DRTEST_ASSERT(verified);
    int a{};
    mock.func(a);
    DRTEST_ASSERT(not mock.mock.verify());
    verified = mock.mock.func<int>().verify();
    DRTEST_ASSERT(not verified);
  }

  {
    VoidFuncOverloadMock mock{};
    bool verified = mock.mock.func<int, float, std::tuple<float, int>>().verify();
    DRTEST_ASSERT(verified);
    int a1{};
    float a2{};
    auto a3 = std::make_tuple<float, int>(12.3f, 456);
    mock.func(a1, a2, a3);
    DRTEST_ASSERT(not mock.mock.verify());
    verified = mock.mock.func<int, float, std::tuple<float, int>>().verify();
    DRTEST_ASSERT(not verified);
  }
}

DRTEST_TEST(success)
{
  {
    VoidFuncOverloadMock mock{};
    mock.mock.func<>().push()
        .times(1);
    mock.func();
    DRTEST_ASSERT(mock.mock.verify());
    DRTEST_ASSERT(mock.mock.func<>().verify());
  }

  {
    VoidFuncOverloadMock mock{};
    int a{5};
    mock.mock.func<int>().push()
        .expects(a)
        .times(1);
    mock.func(a);
    DRTEST_ASSERT(mock.mock.verify());
    DRTEST_ASSERT(mock.mock.func<int>().verify());
  }

  {
    VoidFuncOverloadMock mock{};
    int a1{3};
    float a2{4.5f};
    auto a3 = std::make_tuple<float, int>(12.3f, 456);
    mock.mock.func<int, float, std::tuple<float, int>>().push()
        .expects(a1, a2, a3)
        .times(1);
    mock.func(a1, a2, a3);
    DRTEST_ASSERT(mock.mock.verify());
    DRTEST_ASSERT(
        (mock.mock.func<int, float, std::tuple<float, int>>().verify())
      );
  }
}
