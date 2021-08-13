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

#ifndef DRTEST_MOCK_IMPLEMENTATIONS_GENERATED_FROM_USERS_MALTE_MOCK_TESTS_MOCKER_IVOIDFUNC_H
#define DRTEST_MOCK_IMPLEMENTATIONS_GENERATED_FROM_USERS_MALTE_MOCK_TESTS_MOCKER_IVOIDFUNC_H

#include "/Users/malte/mock/tests/mocker/IVoidFunc.h"
#include "mock/Method.h"
#include "mock/MethodCollection.h"
#include "mock/Qualifiers.h"
#include "mock/Util.h"

extern template class DrMock::Method<void>;

namespace drmock { namespace mock_implementations {

class DRTEST_Object_IVoidFunc
{
private:
  std::shared_ptr<Method<void>> DRTEST_f_0{std::make_shared<Method<void>>()};
  auto& f_dispatch(mock::TypeContainer<>)
  {
    return *DRTEST_f_0;
  }
  MethodCollection methods{std::vector<std::shared_ptr<IMethod>>{DRTEST_f_0}};
  friend class outer::inner::IVoidFunc;

public:
  template<typename ... DRTEST_INNER_Ts>
  auto & f()
  {
    return f_dispatch(mock::TypeContainer<DRTEST_INNER_Ts ...>{});
  }
  bool verify()
  {
    return methods.verify();
  }
};

}}} // namespace DrMock::mock_implementations

namespace outer { namespace inner {

class VoidFuncMock final : public IVoidFunc
{

public:
  mutable DrMock::mock_implementations::DRTEST_Object_IVoidFunc mock{};
  void f() override
  {
    mock.f().call();
  }
};

}} // namespace outer::inner

#endif /* DRTEST_MOCK_IMPLEMENTATIONS_GENERATED_FROM_USERS_MALTE_MOCK_TESTS_MOCKER_IVOIDFUNC_H */
