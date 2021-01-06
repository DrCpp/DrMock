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

#include <drmock/test/Test.h>
#include "mock/TemplateOverloadMock.h"

using namespace outer::inner;

DRTEST_TEST(fails)
{
  {
    TemplateOverloadMock<int, float, double> mock{};
    bool verified = mock.mock.funcTemplateParameters<int, int>().verify();
    DRTEST_ASSERT(verified);
    mock.funcTemplateParameters(int{}, int{});
    DRTEST_ASSERT(not mock.mock.verify());
    verified = mock.mock.funcTemplateParameters<int, int>().verify();
    DRTEST_ASSERT(not verified);
  }

  {
    TemplateOverloadMock<int, float, double> mock{};
    bool verified = mock.mock.funcTemplateParameters<float&&, double&&>().verify();
    DRTEST_ASSERT(verified);
    mock.funcTemplateParameters(float{}, double{});
    DRTEST_ASSERT(not mock.mock.verify());
    verified = mock.mock.funcTemplateParameters<float&&, double&&>().verify();
    DRTEST_ASSERT(not verified);
  }

  {
    TemplateOverloadMock<int, float, double> mock{};
    bool verified = mock.mock.funcTemplateParameters<const int&, float&&, double&&>().verify();
    DRTEST_ASSERT(verified);
    mock.funcTemplateParameters(int{}, float{}, double{});
    DRTEST_ASSERT(not mock.mock.verify());
    verified = mock.mock.funcTemplateParameters<const int&, float&&, double&&>().verify();
    DRTEST_ASSERT(not verified);
  }

  {
    TemplateOverloadMock<int, float, double> mock{};
    bool verified = mock.mock.funcTemplateParameters<
        int&&, const int&, float&&, double&&
      >().verify();
    DRTEST_ASSERT(verified);
    mock.funcTemplateParameters(int{}, int{}, float{}, double{});
    DRTEST_ASSERT(not mock.mock.verify());
    verified = mock.mock.funcTemplateParameters<
        int&&, const int&, float&&, double&&
      >().verify();
    DRTEST_ASSERT(not verified);
  }
}

DRTEST_TEST(success)
{
  {
    TemplateOverloadMock<int, float, double> mock{};
    int a1 = 12;
    int a2 = 13;
    mock.mock.funcTemplateParameters<int, int>().push()
        .expects(a1, a2)
        .times(1);
    mock.funcTemplateParameters(a1, a2);
    DRTEST_ASSERT(mock.mock.verify());
    bool verified = mock.mock.funcTemplateParameters<int, int>().verify();
    DRTEST_ASSERT(verified);
  }

  {
    TemplateOverloadMock<int, float, double> mock{};
    float a1 = 1.2f;
    double a2 = 1.3;
    mock.mock.funcTemplateParameters<float&&, double&&>().push()
        .expects(a1, a2);
    mock.funcTemplateParameters(std::move(a1), std::move(a2));
    DRTEST_ASSERT(mock.mock.verify());
    bool verified = mock.mock.funcTemplateParameters<float&&, double&&>().verify();
    DRTEST_ASSERT(verified);
  }

  {
    TemplateOverloadMock<int, float, double> mock{};
    int a1 = 12;
    float a2 = 1.3f;
    double a3 = 1.4;
    mock.mock.funcTemplateParameters<const int&, float&&, double&&>().push()
        .expects(a1, a2, a3)
        .returns(10)
        .times(1);
    mock.funcTemplateParameters(a1, std::move(a2), std::move(a3));
    DRTEST_ASSERT(mock.mock.verify());
    bool verified = mock.mock.funcTemplateParameters<const int&, float&&, double&&>().verify();
    DRTEST_ASSERT(verified);
  }

  {
    TemplateOverloadMock<int, float, double> mock{};
    int a1 = 12;
    int a2 = 13;
    float a3 = 1.4f;
    double a4 = 1.5;
    mock.mock.funcTemplateParameters<
        int&&, const int&, float&&, double&&
      >().push()
          .expects(a1, a2, a3, a4)
          .times(1);
    mock.funcTemplateParameters(std::move(a1), a2, std::move(a3), std::move(a4));
    DRTEST_ASSERT(mock.mock.verify());
    bool verified = mock.mock.funcTemplateParameters<
        int&&, const int&, float&&, double&&
      >().verify();
    DRTEST_ASSERT(verified);
  }
}
