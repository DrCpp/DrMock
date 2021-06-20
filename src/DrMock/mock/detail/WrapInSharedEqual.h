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

#ifndef DRMOCK_SRC_DRMOCK_MOCK_DETAIL_WRAPINSHAREDEQUAL_H
#define DRMOCK_SRC_DRMOCK_MOCK_DETAIL_WRAPINSHAREDEQUAL_H

#include <memory>
#include <tuple>
#include <type_traits>

#include <DrMock/mock/detail/IWrapInSharedEqual.h>
#include <DrMock/mock/Equal.h>
#include <DrMock/mock/ICompare.h>

namespace drmock { namespace detail {

template<typename Base, typename Derived = Base>
struct WrapInSharedEqual;

template<typename... Bases, typename... Deriveds>
struct WrapInSharedEqual<std::tuple<Bases...>, std::tuple<Deriveds...>> : public IWrapInSharedEqual<Bases...>
{
  static_assert(sizeof...(Bases) == sizeof...(Deriveds));

  std::tuple<std::shared_ptr<ICompare<Bases>>...>
  wrap(expect_t<Bases>&&... pack)
  {
    return std::make_tuple(wrap_in_shared_equal<Bases, Deriveds>(std::forward<expect_t<Bases>>(pack))...);
  }

private:
  template<typename Base, typename Derived = Base>
  std::shared_ptr<ICompare<Base>>
  wrap_in_shared_equal(expect_t<Base>&& var)
  {
    if (std::holds_alternative<Base>(var))
    {
      return std::make_shared<Equal<Base, Derived>>(std::get<Base>(std::forward<expect_t<Base>>(var)));
    }
    else
    {
      return std::get<std::shared_ptr<ICompare<Base>>>(std::forward<expect_t<Base>>(var));
    }
  }
};

}} // namespace drmock::detail

#endif /* DRMOCK_SRC_DRMOCK_MOCK_DETAIL_WRAPINSHAREDEQUAL_H */
