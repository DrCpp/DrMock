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

DRTEST_TEST(params_and_qualifiers)
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
  bar->mock.f<float, const std::vector<int>&, drmock::Const>().push()
      .expects(1.2f, {1, 2, 3})
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
      bar->f(1.2f, {1, 2, 3}),
      4
    );
}

DRTEST_TEST(qualifiers_only)
{
  auto bar = std::make_shared<BarMock>();
  bar->mock.g<>().push()
      .expects({0, 1})
      .returns(5);
  bar->mock.g<drmock::Const>().push()
      .expects({0, 1})
      .returns(6);
  std::vector<int> vec{0, 1};

  DRTEST_ASSERT_EQ(
      bar->g(vec),
      5
    );
  DRTEST_ASSERT_EQ(
      std::const_pointer_cast<const BarMock>(bar)->g(vec),
      6
    );
}

DRTEST_TEST(params_only)
{
  auto bar = std::make_shared<BarMock>();
  bar->mock.h<>().push()
      .expects()
      .returns(1.2f);
  bar->mock.h<int, float>().push()
      .expects(0, 1.2f)
      .returns(3.4f);

  DRTEST_ASSERT_EQ(
      bar->h(),
      1.2f
    );
  DRTEST_ASSERT_EQ(
      bar->h(0, 1.2f),
      3.4f
    );
}
