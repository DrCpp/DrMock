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

#ifndef DRMOCK_SRC_MOCK_DETAIL_ZIP_H
#define DRMOCK_SRC_MOCK_DETAIL_ZIP_H

namespace drmock { namespace detail {

template<
    typename T,  // Tuple
    typename U,  // Tuple
    template<typename, typename> typename Zipper
  >
struct Zip;  // undefined

// Struct that zips the types T1, ..., Tn of T = std::tuple<T1, ..., Tn> and
// U = std::tuple<U1, ..., Un> using a template struct Zipper<typename, typename>.
// In other words: For all types T1, ..., Tn, U1, ..., Un
//
// std::is_same<
//     Zip<std::tuple<T1, ..., Tn>, std::tuple<U1, ..., Un>, Zipper>::type,
//     std::tuple<Zipper<T1, U1>, ..., Zipper<Tn, Un>>
//   >::value
//
// will always equal true.
template<
    typename... TArgs, 
    typename... UArgs,
    template<typename, typename> typename Zipper
  >
struct Zip<std::tuple<TArgs...>, std::tuple<UArgs...>, Zipper>
{
  using type = std::tuple<Zipper<TArgs, UArgs>...>;
};

}} // namespace drmock::detail

#endif /* DRMOCK_SRC_MOCK_DETAIL_ZIP_H */
