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
#include "mock/QualifierMock.h"

using namespace outer::inner;

DRTEST_TEST(fails)
{
  {
    QualifierMock mock{};
    bool verified = mock.mock.f().verify();
    DRTEST_ASSERT(verified);
    mock.f();
    DRTEST_ASSERT(not mock.mock.control.verify());
    verified = mock.mock.f().verify();
    DRTEST_ASSERT(not verified);
  }

  {
    QualifierMock mock{};
    bool verified = mock.mock.g().verify();
    DRTEST_ASSERT(verified);
    mock.g();
    DRTEST_ASSERT(not mock.mock.control.verify());
    verified = mock.mock.g().verify();
    DRTEST_ASSERT(not verified);
  }

  {
    QualifierMock mock{};
    bool verified = mock.mock.h().verify();
    DRTEST_ASSERT(verified);
    float a1{};
    double a2{};
    mock.h(a1, a2);
    DRTEST_ASSERT(not mock.mock.control.verify());
    verified = mock.mock.h().verify();
    DRTEST_ASSERT(not verified);
  }

  {
    QualifierMock mock{};
    bool verified = mock.mock.template get<drmock::LValueRef>().verify();
    DRTEST_ASSERT(verified);
    mock.get();
    DRTEST_ASSERT(not mock.mock.control.verify());
    verified = mock.mock.template get<drmock::LValueRef>().verify();
    DRTEST_ASSERT(not verified);
  }

  {
    const QualifierMock mock{};
    bool verified = mock.mock.template get<drmock::Const, drmock::LValueRef>().verify();
    DRTEST_ASSERT(verified);
    mock.get();
    DRTEST_ASSERT(not mock.mock.control.verify());
    verified = mock.mock.template get<drmock::Const, drmock::LValueRef>().verify();
    DRTEST_ASSERT(not verified);
  }

  {
    QualifierMock mock{};
    bool verified = mock.mock.template get<drmock::RValueRef>().verify();
    DRTEST_ASSERT(verified);
    std::move(mock).get();
    DRTEST_ASSERT(not mock.mock.control.verify());
    verified = mock.mock.template get<drmock::RValueRef>().verify();
    DRTEST_ASSERT(not verified);
  }
}

DRTEST_TEST(success)
{
  {
    QualifierMock mock{};
    mock.mock.f().push()
        .times(1);
    mock.f();
    DRTEST_ASSERT(mock.mock.control.verify());
    DRTEST_ASSERT(mock.mock.f().verify());
  }

  {
    QualifierMock mock{};
    int a{7};
    mock.mock.g().push()
        .returns(a)
        .times(1);
    DRTEST_COMPARE(mock.g(), a);
    DRTEST_ASSERT(mock.mock.control.verify());
    DRTEST_ASSERT(mock.mock.g().verify());
  }

  {
    QualifierMock mock{};
    int r{5};
    float a1{1.23f};
    double a2{45.6};
    mock.mock.h().push()
        .expects(a1, a2)
        .returns(r)
        .times(1);
    DRTEST_COMPARE(mock.h(a1, a2), r);
    DRTEST_ASSERT(mock.mock.control.verify());
    DRTEST_ASSERT(mock.mock.h().verify());
  }

  {
    int value = 3;
    int new_value = 5;
    QualifierMock mock{};
    mock.mock.template get<drmock::LValueRef>().push()
        .returns(value)
        .times(2);
    int& result = mock.get();
    DRTEST_ASSERT_EQ(result, value);
    // Check that changing the result changes the mock!
    result = new_value;
    result = mock.get();
    DRTEST_ASSERT_EQ(result, new_value);
    DRTEST_ASSERT(mock.mock.control.verify());
    DRTEST_ASSERT((mock.mock.template get<drmock::LValueRef>().verify()));
    DRTEST_ASSERT((mock.mock.template get<drmock::Const, drmock::LValueRef>().verify()));
    DRTEST_ASSERT((mock.mock.template get<drmock::RValueRef>().verify()));
  }

  {
    int value = 3;
    const QualifierMock mock{};
    mock.mock.template get<drmock::Const, drmock::LValueRef>().push()
        .returns(value)
        .times(1);
    const int& result = mock.get();
    DRTEST_ASSERT_EQ(result, value);
    DRTEST_ASSERT(mock.mock.control.verify());
    auto verified = mock.mock.template get<drmock::Const, drmock::LValueRef>().verify();
    DRTEST_ASSERT(verified);
  }

  {
    int value = 3;
    QualifierMock mock{};
    mock.mock.template get<drmock::RValueRef>().push()
        .returns(value)
        .times(1);
    auto&& result = std::move(mock).get();
    DRTEST_ASSERT_EQ(result, value);
    DRTEST_ASSERT(mock.mock.control.verify());
    auto verified = mock.mock.template get<drmock::RValueRef>().verify();
    DRTEST_ASSERT(verified);
  }
}
