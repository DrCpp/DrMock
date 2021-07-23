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

#ifndef DRMOCK_SRC_DRMOCK_MOCK_DETAIL_MAKETUPLEOFMATCHERS_H
#define DRMOCK_SRC_DRMOCK_MOCK_DETAIL_MAKETUPLEOFMATCHERS_H

#include <memory>
#include <tuple>
#include <type_traits>

#include <DrMock/mock/detail/IMakeTupleOfMatchers.h>
#include <DrMock/mock/Equal.h>
#include <DrMock/mock/IMatcher.h>

namespace drmock { namespace detail {

template<typename Base, typename Derived = Base>
struct MakeTupleOfMatchers;

template<typename... Bases, typename... Deriveds>
struct MakeTupleOfMatchers<std::tuple<Bases...>, std::tuple<Deriveds...>>
:
  public IMakeTupleOfMatchers<Bases...>
{
  static_assert(sizeof...(Bases) == sizeof...(Deriveds));

  // Transform a tuple of `Variant<Args, IMatcher<Args>>...` into a
  // tuple of `std::shared_ptr<IMatcher<Args>>` by wrapping all "naked"
  // `Args` into `IsEqual<Bases, Deriveds>`.
  std::tuple<std::shared_ptr<IMatcher<Bases>>...>
  wrap(expect_t<Bases>&&... pack)
  {
    return std::make_tuple(wrap_impl<Bases, Deriveds>(std::forward<expect_t<Bases>>(pack))...);
  }

private:
  template<typename Base, typename Derived = Base>
  std::shared_ptr<IMatcher<Base>>
  wrap_impl(expect_t<Base>&& var)
  {
    if (var.template holdsAlternative<Base>())
    {
      return std::make_shared<Equal<Base, Derived>>(std::forward<expect_t<Base>>(var).template get<Base>());
    }
    else
    {
      return std::forward<expect_t<Base>>(var).template get<std::shared_ptr<IMatcher<Base>>>();
    }
  }
};

}} // namespace drmock::detail

#endif /* DRMOCK_SRC_DRMOCK_MOCK_DETAIL_MAKETUPLEOFMATCHERS_H */
