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

#ifndef DRMOCK_SRC_MOCK_DETAIL_INVOKE_ON_PACK_H
#define DRMOCK_SRC_MOCK_DETAIL_INVOKE_ON_PACK_H

#include "../ICompare.h"

namespace drmock { namespace detail {

template<typename Bases>
struct InvokeOnPack;

template<typename... Bases>
struct InvokeOnPack<std::tuple<Bases...>>
{
  bool operator() (
      const std::tuple<std::shared_ptr<ICompare<Bases>>...>& tuple,
      const Bases&... pack
    ) const
  {
    return impl(
        tuple,
        std::make_tuple(std::reference_wrapper<const Bases>{pack}...),
        std::make_index_sequence<sizeof...(Bases)>{}
      );
  }

private:
  template<std::size_t... Is>
  bool impl(
      const std::tuple<std::shared_ptr<ICompare<Bases>>...>& lhs,
      const std::tuple<std::reference_wrapper<const Bases>...>& rhs,
      const std::index_sequence<Is...>&
    ) const
  {
    return (std::get<Is>(lhs)->invoke(std::get<Is>(rhs)) and ...);
  }
};

}} // namespace drmock::detail

#endif /* DRMOCK_SRC_MOCK_DETAIL_INVOKE_ON_PACK_H */
