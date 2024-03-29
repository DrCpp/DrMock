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
#include "mock/CurioTemplateMock.h"

using namespace outer::inner;

DRTEST_TEST(fails)
{
  {
    CurioTemplateMock<int> mock{};
    DRTEST_ASSERT(mock.mock.funcUsingParameter().verify());
    mock.funcUsingParameter({});
    DRTEST_ASSERT(not mock.mock.control.verify());
    DRTEST_ASSERT(not mock.mock.funcUsingParameter().verify());
  }

  {
    CurioTemplateMock<int, float, double> mock{};
    DRTEST_ASSERT(mock.mock.funcOddParameters<const int* const>().verify());
    const int* const a1{};
    mock.funcOddParameters(a1);
    DRTEST_ASSERT(not mock.mock.control.verify());
    DRTEST_ASSERT(not mock.mock.funcOddParameters<const int* const>().verify());
  }

  {
    CurioTemplateMock<int, float, double> mock{};
    DRTEST_ASSERT(mock.mock.funcOddParameters<const int* const* const&&>().verify());
    const int* const* const a1{};
    mock.funcOddParameters(std::move(a1));
    DRTEST_ASSERT(not mock.mock.control.verify());
    DRTEST_ASSERT(not mock.mock.funcOddParameters<const int* const* const&&>().verify());
  }

  {
    CurioTemplateMock<int, float, double> mock{};
    DRTEST_ASSERT(mock.mock.funcNonCopyableArg().verify());
    mock.funcNonCopyableArg({});
    DRTEST_ASSERT(not mock.mock.control.verify());
    DRTEST_ASSERT(not mock.mock.funcNonCopyableArg().verify());
  }

  {
    CurioTemplateMock<int, float, double> mock{};
    DRTEST_ASSERT(mock.mock.funcVariadicParameter().verify());
    const float* x = new float{1.0f};
    const double* y = new double{1.0};
    mock.funcVariadicParameter(std::move(x), std::move(y));
    DRTEST_ASSERT(not mock.mock.control.verify());
    DRTEST_ASSERT(not mock.mock.funcVariadicParameter().verify());
  }

  {
    CurioTemplateMock<int, float, double> mock{};
    DRTEST_ASSERT(mock.mock.funcNonCopyableResult().verify());
    mock.funcNonCopyableResult();
    DRTEST_ASSERT(not mock.mock.control.verify());
    DRTEST_ASSERT(not mock.mock.funcNonCopyableResult().verify());
  }

  {
    CurioTemplateMock<int, float, double> mock{};
    DRTEST_ASSERT(mock.mock.funcNonCopyableResultAsReference().verify());
    mock.funcNonCopyableResultAsReference();
    DRTEST_ASSERT(not mock.mock.control.verify());
    DRTEST_ASSERT(not mock.mock.funcNonCopyableResultAsReference().verify());
  }
}

DRTEST_TEST(success)
{
  {
    CurioTemplateMock<int, float, double> mock{};
    OuterTypeAlias a1{"foo"};
    outer::inner::ICurioTemplate<int, float, double>::InnerTypeAlias r{ 1, 2, 3, 4, 5 };
    mock.mock.funcUsingParameter().push()
        .expects(a1)
        .returns(r)
        .times(1);
    DRTEST_ASSERT(mock.funcUsingParameter(a1) == r);
    DRTEST_ASSERT(mock.mock.control.verify());
    DRTEST_ASSERT(mock.mock.funcUsingParameter().verify());
  }

  {
    CurioTemplateMock<int, float, double> mock{};
    const int* const expects = new int{123};
    mock.mock.funcOddParameters<const int* const>().push()
        .expects(expects)
        .times(1);
    const int* const a1 = new int{123};
    mock.funcOddParameters(a1);
    DRTEST_ASSERT(mock.mock.control.verify());
    DRTEST_ASSERT(mock.mock.funcOddParameters<const int* const>().verify());
    delete a1;
    delete expects;
  }

  {
    CurioTemplateMock<int, float, double> mock{};
    const int* const* expects = new int*{new int{123}};
    mock.mock.funcOddParameters<const int* const* const&&>().push()
        .expects(expects)
        .times(1);
    const int* const* a1 = new int*{new int{123}};
    mock.funcOddParameters(std::move(a1));
    DRTEST_ASSERT(mock.mock.control.verify());
    DRTEST_ASSERT(mock.mock.funcOddParameters<const int* const* const&&>().verify());
    delete *a1;
    delete a1;
    delete *expects;
    delete expects;
  }

  {
    CurioTemplateMock<int, float, double> mock{};
    std::unique_ptr<int> a1 = std::make_unique<int>(12);
    mock.mock.funcNonCopyableArg().push()
        .expects(std::move(a1))
        .times(1);
    a1 = std::make_unique<int>(12);
    mock.funcNonCopyableArg(std::move(a1));
    DRTEST_ASSERT(mock.mock.control.verify());
    DRTEST_ASSERT(mock.mock.funcNonCopyableArg().verify());
  }

  {
    CurioTemplateMock<int, float, double> mock{};
    DRTEST_ASSERT(mock.mock.funcVariadicParameter().verify());
    const float* x = nullptr;
    const double* y = nullptr;
    x = new float{1.0f};
    y = new double{1.0};
    mock.mock.funcVariadicParameter().push()
        .expects(std::move(x), std::move(y))
        .times(1);
    x = new float{1.0f};
    y = new double{1.0};
    mock.funcVariadicParameter(std::move(x), std::move(y));
    DRTEST_ASSERT(mock.mock.control.verify());
    DRTEST_ASSERT(mock.mock.funcVariadicParameter().verify());
  }

  {
    CurioTemplateMock<int, float, double> mock{};
    std::unique_ptr<int> r = std::make_unique<int>(12);
    mock.mock.funcNonCopyableResult().push()
        .expects()
        .returns(std::move(r));
    DRTEST_COMPARE(*mock.funcNonCopyableResult(), 12);
    DRTEST_ASSERT(mock.mock.control.verify());
    DRTEST_ASSERT(mock.mock.funcNonCopyableResult().verify());
  }

  {
    CurioTemplateMock<int, float, double> mock{};
    std::unique_ptr<int> r = std::make_unique<int>(12);
    mock.mock.funcNonCopyableResult().push()
        .expects()
        .returns(std::move(r));
    DRTEST_COMPARE(*mock.funcNonCopyableResult(), 12);
    DRTEST_ASSERT(mock.mock.control.verify());
    DRTEST_ASSERT(mock.mock.funcNonCopyableResult().verify());
  }
}
