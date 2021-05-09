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

#ifndef DRMOCK_SRC_DRMOCK_UTILITY_TYPETRAITS_H
#define DRMOCK_SRC_DRMOCK_UTILITY_TYPETRAITS_H

#include <functional>
#include <memory>
#include <type_traits>

namespace drutility { namespace detail {

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

}} // namespace drutility::detail

#endif /* DRMOCK_SRC_DRMOCK_UTILITY_TYPETRAITS_H */
