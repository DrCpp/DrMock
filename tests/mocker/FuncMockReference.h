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

#ifndef DRTEST_MOCK_IMPLEMENTATIONS_GENERATED_FROM_USERS_MALTE_MOCK_TESTS_MOCKER_IFUNC_H
#define DRTEST_MOCK_IMPLEMENTATIONS_GENERATED_FROM_USERS_MALTE_MOCK_TESTS_MOCKER_IFUNC_H

#include "/Users/malte/mock/tests/mocker/IFunc.h"
#include "mock/Method.h"
#include "mock/MethodCollection.h"
#include "mock/Qualifiers.h"
#include "mock/Util.h"

namespace drmock { namespace mock_implementations {

class DRTEST_Object_IFunc
{
private:
  std::shared_ptr<Method<std :: vector < float >, std :: shared_ptr < std :: unordered_map < int , std :: string >>, float, std :: string>> DRTEST_gParameters_0{std::make_shared<Method<std :: vector < float >, std :: shared_ptr < std :: unordered_map < int , std :: string >>, float, std :: string>>()};
  std::shared_ptr<Method<void, int, std :: vector < float >>> DRTEST_fParameters_0{std::make_shared<Method<void, int, std :: vector < float >>>()};
  auto& gParameters_dispatch(mock::TypeContainer<const std :: shared_ptr < std :: unordered_map < int , std :: string >> &, float, std :: string>)
  {
    return *DRTEST_gParameters_0;
  }
  auto& fParameters_dispatch(mock::TypeContainer<int, std :: vector < float >>)
  {
    return *DRTEST_fParameters_0;
  }
  MethodCollection methods{std::vector<std::shared_ptr<IMethod>>{DRTEST_gParameters_0, DRTEST_fParameters_0}};
  friend class outer::inner::IFunc;

public:
  template<typename ... DRTEST_INNER_Ts>
  auto & gParameters()
  {
    return gParameters_dispatch(mock::TypeContainer<DRTEST_INNER_Ts ..., const std :: shared_ptr < std :: unordered_map < int , std :: string >> &, float, std :: string>{});
  }
  template<typename ... DRTEST_INNER_Ts>
  auto & fParameters()
  {
    return fParameters_dispatch(mock::TypeContainer<DRTEST_INNER_Ts ..., int, std :: vector < float >>{});
  }
  bool verify()
  {
    return methods.verify();
  }
};

}}} // namespace DrMock::mock_implementations

namespace outer { namespace inner {

class FuncMock final : public IFunc
{

public:
  mutable DrMock::mock_implementations::DRTEST_Object_IFunc mock{};
  std :: vector < float > gParameters(const std :: shared_ptr < std :: unordered_map < int , std :: string >> & a0, float a1, std :: string a2) override
  {
    auto& result = *mock.gParameters().call(a0, std::move(a1), std::move(a2));
    return std::forward<std :: vector < float >>(DrMock::moveIfNotCopyConstructible(result));
  }
  void fParameters(int a0, std :: vector < float > a1) override
  {
    mock.fParameters().call(std::move(a0), std::move(a1));
  }
};

}} // namespace outer::inner

#endif /* DRTEST_MOCK_IMPLEMENTATIONS_GENERATED_FROM_USERS_MALTE_MOCK_TESTS_MOCKER_IFUNC_H */
