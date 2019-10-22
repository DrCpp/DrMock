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

#include "test/Test.h"
#include "mock/mocker/TemplateMock.h"

using namespace outer::inner;

DRTEST_TEST(fails)
{
  {
    TemplateMock<int, float, double> mock{};
    bool verified = mock.mock.fTemplate().verify();
    DRTEST_ASSERT(verified);
    mock.fTemplate(float{}, double{});
    DRTEST_ASSERT(not mock.mock.verify());
    verified = mock.mock.fTemplate().verify();
    DRTEST_ASSERT(not verified);
  }

  {
    TemplateMock<int, float, double> mock{};
    bool verified = mock.mock.gTemplate().verify();
    DRTEST_ASSERT(verified);
    mock.gTemplate(int{}, float{}, double{});
    DRTEST_ASSERT(not mock.mock.verify());
    verified = mock.mock.gTemplate().verify();
    DRTEST_ASSERT(not verified);
  }

  {
    TemplateMock<int, float, double> mock{};
    bool verified = mock.mock.hTemplate().verify();
    DRTEST_ASSERT(verified);
    int a1{};
    int a2{};
    float* const a3 = new float{123};
    double* const a4 = new double{123};
    mock.hTemplate(std::move(a1), a2, a3, a4);
    DRTEST_ASSERT(not mock.mock.verify());
    verified = mock.mock.hTemplate().verify();
    DRTEST_ASSERT(not verified);
  }
}

DRTEST_TEST(succeeds)
{
  {
    TemplateMock<int, float, double> mock{};
    float a1 = 12.0f;
    double a2 = 13.0;
    mock.mock.fTemplate().push()
        .expects(a1, a2)
        .times(1);
    mock.fTemplate(a1, a2);
    DRTEST_ASSERT(mock.mock.verify());
    bool verified = mock.mock.fTemplate().verify();
    DRTEST_ASSERT(verified);
  }

  {
    TemplateMock<int, float, double> mock{};
    int a0 = 1;
    float a1 = 1.2f;
    double a2 = 1.3;
    bool r = true;
    mock.mock.gTemplate().push()
        .expects(a0, a1, a2)
        .returns(r);
    DRTEST_COMPARE(
        mock.gTemplate(std::move(a0), std::move(a1), std::move(a2)),
        r
      );
    DRTEST_ASSERT(mock.mock.verify());
    bool verified = mock.mock.gTemplate().verify();
    DRTEST_ASSERT(verified);
  }

  {
    TemplateMock<int, float, double> mock{};
    int a1 = 12;
    int a2 = 34;
    float* const a3 = new float{1.4f};
    double* const a4 = new double{1.4};
    auto r = std::make_tuple(12, 34, 5.6f, 7.8);
    mock.mock.hTemplate().push()
        .expects(a1, a2, a3, a4)
        .returns(r)
        .times(1);
    auto s = mock.hTemplate(std::move(a1), a2, a3, a4);
    DRTEST_COMPARE(std::get<0>(r), std::get<0>(s));
    DRTEST_COMPARE(std::get<1>(r), std::get<1>(s));
    DRTEST_COMPARE(std::get<2>(r), std::get<2>(s));
    DRTEST_COMPARE(std::get<3>(r), std::get<3>(s));
    DRTEST_ASSERT(mock.mock.verify());
    bool verified = mock.mock.hTemplate().verify();
    DRTEST_ASSERT(verified);
  }
}
