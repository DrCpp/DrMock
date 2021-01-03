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

#ifndef DRMOCK_SRC_MOCK_DETAIL_ISTUPLEPACKEQUAL_H
#define DRMOCK_SRC_MOCK_DETAIL_ISTUPLEPACKEQUAL_H

#include "IIsTuplePackEqual.h"
#include "IsEqual.h"

namespace drmock { namespace detail {

/* IsTuplePackEqual

Class template with operator() that compares lhs tuples with rhs
parameter pack, according to the rules specified in `IsEqual.h`.
*/

template<
    typename TupleTypeBase,
    typename TupleTypeDerived = TupleTypeBase,
    std::size_t I = 0
  >
struct IsTuplePackEqual;

template<
    std::size_t I,
    typename TupleTypeBase,
    typename TupleTypeDerived,
    typename T,
    typename... Ts
  >
struct IsTuplePackEqualImpl;

template<
    typename... Bases,
    typename... Deriveds
  >
struct IsTuplePackEqual<std::tuple<Bases...>, std::tuple<Deriveds...>, 0> final :
    public IIsTuplePackEqual<Bases...>
{
  bool operator() (
      const std::tuple<Bases...>& tuple,
      const Bases&... pack
    ) override
  {
    return impl<0>(tuple, std::make_tuple(std::reference_wrapper<const Bases>{pack}...));
  }

private:
  template<std::size_t I>
  bool impl(
      const std::tuple<Bases...>& lhs,
      const std::tuple<std::reference_wrapper<const Bases>...>& rhs
    )
  {
    if constexpr (I < sizeof...(Bases))
    {
      IsEqual<
          typename std::tuple_element<I, std::tuple<Bases...>>::type,
          typename std::tuple_element<I, std::tuple<Deriveds...>>::type
        > is_equal{};
      return is_equal(std::get<I>(lhs), std::get<I>(rhs).get()) and impl<I + 1>(lhs, rhs);
    }
    else
    {
      return true;
    }
  }
};

}} // namespaces

#endif /* DRMOCK_SRC_MOCK_DETAIL_ISTUPLEPACKEQUAL_H */
