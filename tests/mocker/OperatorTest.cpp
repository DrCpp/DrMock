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

#include <DrMock/test/Test.h>
#include "mock/OperatorMock.h"

using namespace outer::inner;

DRTEST_TEST(fails)
{
  {
    OperatorMock mock{};
    bool verified = mock.mock.operatorEqual().verify();
    DRTEST_ASSERT(verified);
    (void)(mock == int{});  // Call the operator, mark result as unused!
    DRTEST_ASSERT(not mock.mock.verify());
    verified = mock.mock.operatorEqual().verify();
    DRTEST_ASSERT(not verified);
  }

  {
    OperatorMock mock{};
    bool verified = mock.mock.operatorPlus().verify();
    DRTEST_ASSERT(verified);
    std::vector<int> a{};
    mock + a;  // Call the operator.
    DRTEST_ASSERT(not mock.mock.verify());
    verified = mock.mock.operatorPlus().verify();
    DRTEST_ASSERT(not verified);
  }

  {
    OperatorMock mock{};
    bool verified = mock.mock.operatorCall().verify();
    DRTEST_ASSERT(verified);
    int a1{};
    float a2{};
    double* const a3 = new double{123};
    mock(a1, a2, a3);  // Call the operator.
    DRTEST_ASSERT(not mock.mock.verify());
    verified = mock.mock.operatorCall().verify();
    DRTEST_ASSERT(not verified);
  }
}

DRTEST_TEST(succeeds)
{
  {
    OperatorMock mock{};
    int a1{3};
    bool r{true};
    mock.mock.operatorEqual().push()
        .expects(a1)
        .returns(r)
        .times(1);
    DRTEST_COMPARE((mock == a1), r);
    DRTEST_ASSERT(mock.mock.verify());
    bool verified = mock.mock.operatorEqual().verify();
    DRTEST_ASSERT(verified);
  }

  {
    OperatorMock mock{};
    std::vector<int> a0{4, 5, 6};
    mock.mock.operatorPlus().push()
        .expects(a0);
    mock + a0;  // Call the operator.
    DRTEST_ASSERT(mock.mock.verify());
    bool verified = mock.mock.operatorPlus().verify();
    DRTEST_ASSERT(verified);
  }

  {
    OperatorMock mock{};
    int a1 = 12;
    float a2 = 3.45;
    double* const a3 = new double{1.4};
    int r = 9;
    mock.mock.operatorCall().push()
        .expects(a1, a2, a3)
        .returns(r)
        .times(1);
    DRTEST_COMPARE(mock(a1, a2, a3), r);
    DRTEST_ASSERT(mock.mock.verify());
    bool verified = mock.mock.operatorCall().verify();
    DRTEST_ASSERT(verified);
  }
}
