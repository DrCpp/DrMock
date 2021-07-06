/* Copyright 2021 Ole Kliemann, Malte Kliemann
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
#include "mock/FuncNotAbstractMock.h"

using namespace outer::inner;

DRTEST_TEST(fails)
{
  {
    FuncNotAbstractMock mock{};
    bool verified = mock.mock.fParameters().verify();
    DRTEST_ASSERT(verified);
    int a1{3};
    std::vector<float> a2{};
    mock.fParameters(a1, a2);
    DRTEST_ASSERT(not mock.mock.verify());
    verified = mock.mock.fParameters().verify();
    DRTEST_ASSERT(not verified);
  }

  {
    FuncNotAbstractMock mock{};
    bool verified = mock.mock.gParameters().verify();
    DRTEST_ASSERT(verified);
    std::shared_ptr<std::unordered_map<int, std::string>> a1{};
    float a2{3};
    std::string a3{};
    mock.gParameters(a1, a2, a3);
    DRTEST_ASSERT(not mock.mock.verify());
    verified = mock.mock.gParameters().verify();
    DRTEST_ASSERT(not verified);
  }

  {
    FuncNotAbstractMock mock{};
    DRTEST_ASSERT(mock.mock.virtualMethodNotPure().verify());
    mock.virtualMethodNotPure();
    DRTEST_ASSERT(not mock.mock.verify());
    DRTEST_ASSERT(not mock.mock.virtualMethodNotPure().verify());
  }
}

DRTEST_TEST(success)
{
  {
    FuncNotAbstractMock mock{};
    int a1{3};
    std::vector<float> a2{};
    mock.mock.fParameters().push()
        .expects(a1, a2)
        .times(1);
    mock.fParameters(a1, a2);
    DRTEST_ASSERT(mock.mock.verify());
    DRTEST_ASSERT(mock.mock.fParameters().verify());
  }

  {
    FuncNotAbstractMock mock{};
    std::shared_ptr<std::unordered_map<int, std::string>> a1
        = std::make_shared<std::unordered_map<int, std::string>>();
    float a2{3};
    std::string a3{"foo"};
    std::vector<float> r{0, 1, 2, 4};
    mock.mock.gParameters().push()
        .expects(a1, a2, a3)
        .returns(r)
        .times(1);
    DRTEST_ASSERT(mock.gParameters(a1, a2, a3) == r);
    DRTEST_ASSERT(mock.mock.verify());
    DRTEST_ASSERT(mock.mock.gParameters().verify());
  }

  {
    FuncNotAbstractMock mock{};
    auto value = 34;
    mock.mock.virtualMethodNotPure().push().returns(value).times(1);
    DRTEST_ASSERT(mock.virtualMethodNotPure() == value);
    DRTEST_ASSERT(mock.mock.verify());
    DRTEST_ASSERT(mock.mock.virtualMethodNotPure().verify());
  }

  // Check that the non-virtual methods are not overloaded.
  {
    FuncNotAbstractMock mock{};
    DRTEST_ASSERT(mock.notVirtual() == 12);
    DRTEST_ASSERT(mock.mock.verify());
  }
}

int passthru(IFuncNotAbstract& obj)
{
  return obj.virtualMethodNotPure();
}

DRTEST_TEST(polymorphicReference)
{
  FuncNotAbstractMock mock{};
  auto value = 34;
  mock.mock.virtualMethodNotPure().push().returns(value).times(1);
  DRTEST_ASSERT(passthru(mock) == 34);
}
