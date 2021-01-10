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
#include "mock/QualifierOverloadMock.h"

using namespace outer::inner;

DRTEST_TEST(fails)
{
  {
    QualifierOverloadMock mock{};
    DRTEST_ASSERT(mock.mock.funcOverloadConst<>().verify());
    mock.funcOverloadConst();
    DRTEST_ASSERT(not mock.mock.verify());
    DRTEST_ASSERT(not mock.mock.funcOverloadConst<>().verify());
  }

  {
    QualifierOverloadMock mock{};
    DRTEST_ASSERT(mock.mock.funcOverloadConst<drmock::Const>().verify());
    const_cast<const QualifierOverloadMock&>(mock).funcOverloadConst();
    DRTEST_ASSERT(not mock.mock.verify());
    DRTEST_ASSERT(not mock.mock.funcOverloadConst<drmock::Const>().verify());
  }

  {
    QualifierOverloadMock mock{};
    DRTEST_ASSERT(mock.mock.funcOverloadConstDifferentReturn<>().verify());
    mock.funcOverloadConstDifferentReturn();
    DRTEST_ASSERT(not mock.mock.verify());
    DRTEST_ASSERT(not mock.mock.funcOverloadConstDifferentReturn<>().verify());
  }

  {
    QualifierOverloadMock mock{};
    DRTEST_ASSERT(
        mock.mock.funcOverloadConstDifferentReturn<drmock::Const>().verify()
      );
    const_cast<const QualifierOverloadMock&>(mock).funcOverloadConstDifferentReturn();
    DRTEST_ASSERT(not mock.mock.verify());
    DRTEST_ASSERT(
        not mock.mock.funcOverloadConstDifferentReturn<drmock::Const>().verify()
      );
  }

  {
    QualifierOverloadMock mock{};
    DRTEST_ASSERT(mock.mock.funcOverloadMultipleConstReturns<>().verify());
    mock.funcOverloadMultipleConstReturns();
    DRTEST_ASSERT(not mock.mock.verify());
    DRTEST_ASSERT(not mock.mock.funcOverloadMultipleConstReturns<>().verify());
  }

  {
    QualifierOverloadMock mock{};
    DRTEST_ASSERT(
        mock.mock.funcOverloadMultipleConstReturns<drmock::Const>().verify()
      );
    const_cast<const QualifierOverloadMock&>(mock).funcOverloadMultipleConstReturns();
    DRTEST_ASSERT(not mock.mock.verify());
    DRTEST_ASSERT(
        not mock.mock.funcOverloadMultipleConstReturns<drmock::Const>().verify()
      );
  }

  {
    QualifierOverloadMock mock{};
    DRTEST_ASSERT(mock.mock.funcOverloadMultipleConstReturns<int>().verify());
    mock.funcOverloadMultipleConstReturns(123);
    DRTEST_ASSERT(not mock.mock.verify());
    DRTEST_ASSERT(not mock.mock.funcOverloadMultipleConstReturns<int>().verify());
  }

  {
    QualifierOverloadMock mock{};
    DRTEST_ASSERT(
        (mock.mock.funcOverloadMultipleConstReturns<int, drmock::Const>().verify())
      );
    const_cast<const QualifierOverloadMock&>(mock).funcOverloadMultipleConstReturns(123);
    DRTEST_ASSERT(not mock.mock.verify());
    DRTEST_ASSERT(
        (not mock.mock.funcOverloadMultipleConstReturns<int, drmock::Const>().verify())
      );
  }
}

DRTEST_TEST(success)
{
  {
    QualifierOverloadMock mock{};
    mock.mock.funcOverloadConst().push()
        .expects()
        .times(1);
    mock.funcOverloadConst();
    DRTEST_ASSERT(mock.mock.verify());
    DRTEST_ASSERT(mock.mock.funcOverloadConst<>().verify());
  }

  {
    QualifierOverloadMock mock{};
    mock.mock.funcOverloadConst<drmock::Const>().push()
        .expects()
        .times(1);
    const_cast<const QualifierOverloadMock&>(mock).funcOverloadConst();
    DRTEST_ASSERT(mock.mock.verify());
    DRTEST_ASSERT(mock.mock.funcOverloadConst<drmock::Const>().verify());
  }

  {
    QualifierOverloadMock mock{};
    std::vector<std::string> r{ "foo", "bar" };
    mock.mock.funcOverloadConstDifferentReturn().push()
        .expects()
        .returns(r)
        .times(1);
    DRTEST_ASSERT(mock.funcOverloadConstDifferentReturn() == r);
    DRTEST_ASSERT(mock.mock.verify());
    DRTEST_ASSERT(mock.mock.funcOverloadConstDifferentReturn<>().verify());
  }

  {
    QualifierOverloadMock mock{};
    std::vector<std::string> r{ "foo", "bar" };
    mock.mock.funcOverloadConstDifferentReturn<drmock::Const>().push()
        .expects()
        .returns(r)
        .times(1);
    auto result = const_cast<
        const QualifierOverloadMock&
      >(mock).funcOverloadConstDifferentReturn();
    DRTEST_ASSERT(result == r);
    DRTEST_ASSERT(mock.mock.verify());
    DRTEST_ASSERT(
        mock.mock.funcOverloadConstDifferentReturn<drmock::Const>().verify()
      );
  }

  {
    QualifierOverloadMock mock{};
    std::vector<std::string> r{ "foo", "bar" };
    mock.mock.funcOverloadMultipleConstReturns().push()
        .expects()
        .returns(r)
        .times(1);
    DRTEST_ASSERT(mock.funcOverloadMultipleConstReturns() == r);
    DRTEST_ASSERT(mock.mock.verify());
    DRTEST_ASSERT(mock.mock.funcOverloadMultipleConstReturns<>().verify());
  }

  {
    QualifierOverloadMock mock{};
    std::vector<std::string> r{ "foo", "bar" };
    mock.mock.funcOverloadMultipleConstReturns<drmock::Const>().push()
        .expects()
        .returns(r)
        .times(1);
    auto result = const_cast<
        const QualifierOverloadMock&
      >(mock).funcOverloadMultipleConstReturns();
    DRTEST_ASSERT(result == r);
    DRTEST_ASSERT(mock.mock.verify());
    DRTEST_ASSERT(
        mock.mock.funcOverloadMultipleConstReturns<drmock::Const>().verify()
      );
  }

  {
    QualifierOverloadMock mock{};
    std::string r{"foo"};
    mock.mock.funcOverloadMultipleConstReturns<int>().push()
        .expects(123)
        .returns(r)
        .times(1);
    DRTEST_ASSERT(mock.funcOverloadMultipleConstReturns(123) == r);
    DRTEST_ASSERT(mock.mock.verify());
    DRTEST_ASSERT(mock.mock.funcOverloadMultipleConstReturns<>().verify());
  }

  {
    QualifierOverloadMock mock{};
    std::string r{"bar"};
    mock.mock.funcOverloadMultipleConstReturns<int, drmock::Const>().push()
        .expects(123)
        .returns(r)
        .times(1);
    auto result = const_cast<
        const QualifierOverloadMock&
      >(mock).funcOverloadMultipleConstReturns(123);
    DRTEST_ASSERT(result == r);
    DRTEST_ASSERT(mock.mock.verify());
    DRTEST_ASSERT(
        mock.mock.funcOverloadMultipleConstReturns<drmock::Const>().verify()
      );
  }
}
