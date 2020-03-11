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

#ifndef DRTEST_MOCK_IMPLEMENTATIONS_GENERATED_FROM_USERS_MALTE_MOCK_TESTS_MOCKER_ITEMPLATE_H
#define DRTEST_MOCK_IMPLEMENTATIONS_GENERATED_FROM_USERS_MALTE_MOCK_TESTS_MOCKER_ITEMPLATE_H

#include "/Users/malte/mock/tests/mocker/ITemplate.h"
#include "mock/Method.h"
#include "mock/MethodCollection.h"
#include "mock/Qualifiers.h"
#include "mock/Util.h"

// Won't work:
extern template class detail::MethodDecayedArgs<std::tuple<T, T, Ts...>, T&&, const int&, Ts* const ...>::type; // ???
// etc.

// Maybe:
extern template class Method<
    std::tuple<T, T, Ts...>,
    std::decay<T&&>::type,
    std::decay<const int&>::type,
    std::decay<Ts* const...>::type
  >;

// Or: Use `mocker` to decay the arguments?

namespace drmock { namespace mock_implementations {

template<typename T, typename ... Ts>
class DRTEST_Object_ITemplate
{
private:
  // Method objects.
  std::shared_ptr<
      typename detail::MethodDecayedArgs<std::tuple<T, T, Ts...>, T&&, const int&, Ts* const ...>::type> DRTEST_hTemplate_0{
      std::make_shared<typename detail::MethodDecayedArgs<std :: tuple < T , T , Ts ... >, T &&, const int &, Ts * const ...>::type>()
    };

  std::shared_ptr<typename detail::MethodDecayedArgs<void, const Ts& ...>::type> DRTEST_fTemplate_0{
      std::make_shared<typename detail::MethodDecayedArgs<void, const Ts & ...>::type>()
    };

  std::shared_ptr<typename detail::MethodDecayedArgs<bool, const T &, Ts && ...>::type> DRTEST_gTemplate_0{
      std::make_shared<typename detail::MethodDecayedArgs<bool, const T &, Ts && ...>::type>()
    };

  // Dispatch methods.
  auto& hTemplate_dispatch(mock::TypeContainer<T&&, const int&, Ts* const ...>)
  {
    return *DRTEST_hTemplate_0;
  }
  auto& fTemplate_dispatch(mock::TypeContainer<const Ts&...>)
  {
    return *DRTEST_fTemplate_0;
  }
  auto& gTemplate_dispatch(mock::TypeContainer<const T&, Ts&&...>)
  {
    return *DRTEST_gTemplate_0;
  }

  MethodCollection methods{
      std::vector<std::shared_ptr<IMethod>>{
          DRTEST_hTemplate_0,
          DRTEST_fTemplate_0,
          DRTEST_gTemplate_0
        }
    };
  friend class outer::inner::ITemplate<T, Ts ...>;

public:
  template<typename ... DRTEST_INNER_Ts>
  auto & hTemplate()
  {
    return hTemplate_dispatch(
        mock::TypeContainer<DRTEST_INNER_Ts ..., T &&, const int &, Ts * const ...>{}
      );
  }
  template<typename ... DRTEST_INNER_Ts>
  auto & fTemplate()
  {
    return fTemplate_dispatch(
        mock::TypeContainer<DRTEST_INNER_Ts ..., const Ts & ...>{}
      );
  }
  template<typename ... DRTEST_INNER_Ts>
  auto & gTemplate()
  {
    return gTemplate_dispatch(
        mock::TypeContainer<DRTEST_INNER_Ts ..., const T &, Ts && ...>{}
      );
  }
  bool verify()
  {
    return methods.verify();
  }
};

}}} // namespace drmock::mock_implementations

namespace outer { namespace inner {

template<typename T, typename ... Ts>
class TemplateMock final : public ITemplate<T, Ts ...>
{
public:
  mutable drmock::mock_implementations::DRTEST_Object_ITemplate<T, Ts ...> mock{};

  std :: tuple < T , T , Ts ... > hTemplate(T && a0, const int & a1, Ts * const ... a2) override
  {
    auto& result = *mock.hTemplate().call(std::move(a0), a1, std::move(a2) ...);
    return std::forward<std :: tuple < T , T , Ts ... >>(
        drmock::moveIfNotCopyConstructible(result)
      );
  }
  void fTemplate(const Ts & ... a0) override
  {
    mock.fTemplate().call(a0 ...);
  }
  bool gTemplate(const T & a0, Ts && ... a1) override
  {
    auto& result = *mock.gTemplate().call(a0, std::move(a1) ...);
    return std::forward<bool>(
        drmock::moveIfNotCopyConstructible(result)
      );
  }
};

}} // namespace outer::inner

#endif /* DRTEST_MOCK_IMPLEMENTATIONS_GENERATED_FROM_USERS_MALTE_MOCK_TESTS_MOCKER_ITEMPLATE_H */
