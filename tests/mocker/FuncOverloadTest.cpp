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
#include "mock/FuncOverloadMock.h"

using namespace outer::inner;

DRTEST_TEST(fails)
{
  {
    FuncOverloadMock mock{};
    DRTEST_ASSERT(
        mock.mock.funcOverloadLvalueRvalueAndConst<const std::string&>().verify()
      );
    const std::string a1{};
    mock.funcOverloadLvalueRvalueAndConst(a1);
    DRTEST_ASSERT(not mock.mock.verify());
    DRTEST_ASSERT(
        not mock.mock.funcOverloadLvalueRvalueAndConst<const std::string&>().verify()
      );
  }

  {
    FuncOverloadMock mock{};
    DRTEST_ASSERT(mock.mock.funcOverloadLvalueRvalueAndConst<std::string&>().verify());
    std::string a1{};
    mock.funcOverloadLvalueRvalueAndConst(a1);
    DRTEST_ASSERT(not mock.mock.verify());
    DRTEST_ASSERT(not mock.mock.funcOverloadLvalueRvalueAndConst<std::string&>().verify());
  }

  {
    FuncOverloadMock mock{};
    DRTEST_ASSERT(
        mock.mock.funcOverloadLvalueRvalueAndConst<const std::string&&>().verify()
      );
    const std::string a1{};
    mock.funcOverloadLvalueRvalueAndConst(std::move(a1));
    DRTEST_ASSERT(not mock.mock.verify());
    DRTEST_ASSERT(
        not mock.mock.funcOverloadLvalueRvalueAndConst<const std::string&&>().verify()
      );
  }

  {
    FuncOverloadMock mock{};
    DRTEST_ASSERT(mock.mock.funcOverloadLvalueRvalueAndConst<std::string&&>().verify());
    std::string a1{};
    mock.funcOverloadLvalueRvalueAndConst(std::move(a1));
    DRTEST_ASSERT(not mock.mock.verify());
    DRTEST_ASSERT(not mock.mock.funcOverloadLvalueRvalueAndConst<std::string&&>().verify());
  }
}

DRTEST_TEST(success)
{
  {
    FuncOverloadMock mock{};
    const std::string a1{"foo"};
    mock.mock.funcOverloadLvalueRvalueAndConst<const std::string&>().push()
        .expects(a1)
        .times(1);
    mock.funcOverloadLvalueRvalueAndConst(a1);
    DRTEST_ASSERT(mock.mock.verify());
    DRTEST_ASSERT(
        mock.mock.funcOverloadLvalueRvalueAndConst<const std::string&>().verify()
      );
  }

  {
    FuncOverloadMock mock{};
    std::string a1{"foo"};
    mock.mock.funcOverloadLvalueRvalueAndConst<std::string&>().push()
        .expects(a1)
        .times(1);
    mock.funcOverloadLvalueRvalueAndConst(a1);
    DRTEST_ASSERT(mock.mock.verify());
    DRTEST_ASSERT(mock.mock.funcOverloadLvalueRvalueAndConst<std::string&>().verify());
  }

  {
    FuncOverloadMock mock{};
    const std::string a1{"foo"};
    bool r = true;
    mock.mock.funcOverloadLvalueRvalueAndConst<const std::string&&>().push()
        .expects(a1)
        .returns(r)    
        .times(1);
    DRTEST_COMPARE(
        mock.funcOverloadLvalueRvalueAndConst(std::move(a1)),
        r
      );
    DRTEST_ASSERT(mock.mock.verify());
    DRTEST_ASSERT(
        mock.mock.funcOverloadLvalueRvalueAndConst<const std::string&&>().verify()
      );
  }

  {
    FuncOverloadMock mock{};
    std::string a1{"foo"};
    bool r = true;
    mock.mock.funcOverloadLvalueRvalueAndConst<std::string&&>().push()
        .expects(a1)
        .returns(r)
        .times(1);
    DRTEST_COMPARE(
        mock.funcOverloadLvalueRvalueAndConst(std::move(a1)),
        r
      );
    DRTEST_ASSERT(mock.mock.verify());
    DRTEST_ASSERT(mock.mock.funcOverloadLvalueRvalueAndConst<std::string&&>().verify());
  }
}
