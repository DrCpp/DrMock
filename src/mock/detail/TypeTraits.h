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

#ifndef DRMOCK_SRC_MOCK_TYPETRAITS_H
#define DRMOCK_SRC_MOCK_TYPETRAITS_H

#include <functional>
#include <memory>
#include <type_traits>

namespace drmock { namespace detail {

template<typename... Ts>
struct is_tuple : std::false_type {};

template<typename... Ts>
struct is_tuple<std::tuple<Ts...>> : std::true_type {};

template<typename... Ts>
struct is_tuple<const std::tuple<Ts...>> : std::true_type {};

template<typename T>
struct is_shared_ptr : std::false_type {};

template<typename T>
struct is_shared_ptr<std::shared_ptr<T>> : std::true_type {};

template<typename T>
struct is_shared_ptr<const std::shared_ptr<T>> : std::true_type {};

template<typename T>
struct is_unique_ptr : std::false_type {};

template<typename T>
struct is_unique_ptr<std::unique_ptr<T>> : std::true_type {};

template<typename T>
struct is_unique_ptr<const std::unique_ptr<T>> : std::true_type {};

template<typename T, typename = std::void_t<>>
struct is_output_streamable : std::false_type {};

template<typename T>
struct is_output_streamable<
    T,
    std::void_t<decltype(std::declval<std::ostream&>() << std::declval<const T&>())>
 > : std::true_type {};

template<typename T1, typename T2>
struct is_base_of_smart_ptr
{
  static constexpr bool value = std::is_base_of_v<T1, T2>;
};

template<typename T1, typename T2>
struct is_base_of_smart_ptr<std::shared_ptr<T1>, std::shared_ptr<T2>>
{
  static constexpr bool value = std::is_base_of_v<T1, T2>;
};

template<typename T1, typename T2>
struct is_base_of_smart_ptr<std::unique_ptr<T1>, std::unique_ptr<T2>>
{
  static constexpr bool value = std::is_base_of_v<T1, T2>;
};

template<typename T1, typename T2>
inline constexpr bool is_base_of_smart_ptr_v = is_base_of_smart_ptr<T1, T2>::value;

template<typename T1, typename T2, bool enable>
struct is_base_of_tuple_impl;

template<typename... Ts1, typename... Ts2>
struct is_base_of_tuple_impl<std::tuple<Ts1...>, std::tuple<Ts2...>, false>
{
  static constexpr bool value = false;
};

template<typename... Ts1, typename... Ts2>
struct is_base_of_tuple_impl<std::tuple<Ts1...>, std::tuple<Ts2...>, true>
{
  static constexpr bool value = (is_base_of_smart_ptr_v<Ts1, Ts2> and ...);
};

template<typename T1, typename T2>
struct is_base_of_tuple : std::false_type {};

template<typename... Ts1, typename... Ts2>
struct is_base_of_tuple<std::tuple<Ts1...>, std::tuple<Ts2...>>
{
  static constexpr bool value = is_base_of_tuple_impl<
      std::tuple<Ts1...>,
      std::tuple<Ts2...>,
      (sizeof...(Ts1) == sizeof...(Ts2))
    >::value;
};

template<typename T1, typename T2>
inline constexpr bool is_base_of_tuple_v = is_base_of_tuple<T1, T2>::value;

}} // namespace drmock

#endif /* DRMOCK_SRC_MOCK_TYPETRAITS_H */
