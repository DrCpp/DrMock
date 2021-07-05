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
#include "mock/BarMock.h"

using namespace drmock::samples;

DRTEST_TEST(overload)
{
  auto bar = std::make_shared<BarMock>();
  bar->mock.f<>().push()
      .expects()
      .returns(1);
  bar->mock.f<drmock::Const>().push()
      .expects()
      .returns(2);
  bar->mock.f<int>().push()
      .expects(3)
      .returns(3);
  bar->mock.f<float, std::vector<int>, drmock::Const>().push()
      .expects(0.0f, {1, 2, 3})
      .returns(4);

  DRTEST_ASSERT_EQ(
      bar->f(),
      1
    );
  DRTEST_ASSERT_EQ(
      std::const_pointer_cast<const BarMock>(bar)->f(),
      2
    );
  DRTEST_ASSERT_EQ(
      bar->f(3),
      3
    );
  DRTEST_ASSERT_EQ(
      bar->f(0.0f, {1, 2, 3}),
      4
    );
}
