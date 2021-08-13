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

#ifndef DRMOCK_SRC_DRMOCK_MOCK_DETAIL_VARIANT_H
#define DRMOCK_SRC_DRMOCK_MOCK_DETAIL_VARIANT_H

#include <type_traits>
#include <utility>
#include <variant>

namespace drmock { namespace detail {

// Variant wrapper which allows implicit conversions for the first
// alternative. If both alternatives can be constructed from a value
// passed to any of the constructors, the first alternative is
// constructed. Only if the first alternative cannot be constructed from
// the value, then the second alternative is constructed.
template<typename T1, typename T2>
class Variant
{
public:
  Variant()
  :
    var_{}
  {}

  template<typename... Ts, typename = std::void_t<decltype(T1{std::declval<Ts>()...})>>
  Variant(Ts&&... ts)
  :
    var_{T1{std::forward<Ts>(ts)...}}  // Need this indirection for implicit conversion.
  {}

  // For list initialization.
  Variant(T1 t)
  :
    var_{std::move(t)}
  {}

  template<typename T, typename = std::enable_if_t<not std::is_constructible_v<T1, T>>>
  Variant(T t)
  :
    var_{std::move(t)}
  {}

  // FIXME This does not behave like the ctors above. Currently, this op
  // is not used, so this is fine.
  template<typename T>
  Variant&
  operator=(const T& rhs)
  {
    var_ = rhs;
  }

  template<typename T>
  Variant&
  operator=(T&& rhs)
  {
    var_ = std::move(rhs);
  }

  template<typename T>
  bool
  holdsAlternative() const
  {
    return std::holds_alternative<T>(var_);
  }

  template<typename T>
  T&
  get() &
  {
    return std::get<T>(var_);
  }

  template<typename T>
  T&&
  get() &&
  {
    return std::move(std::get<T>(var_));
  }

private:
  std::variant<T1, T2> var_;
};

}} // namespace drmock::detail

#endif /* DRMOCK_SRC_DRMOCK_MOCK_DETAIL_VARIANT_H */
