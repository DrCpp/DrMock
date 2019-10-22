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

#ifndef DRMOCK_SRC_MOCK_DETAIL_HASH_H
#define DRMOCK_SRC_MOCK_DETAIL_HASH_H

#include <tuple>

#include "TypeTraits.h"
#include "IHash.h"
#include "Zip.h"

namespace drmock { namespace detail {

// A recursively defined template class similar to `std::hash`. If `Hash<T>`
// is defined, then `Hash<T>{}(t)` returns a hash of the object `t` of type
// `T`; the following specializations of `Hash<T>` are defined.
//
// (1) `Hash<T, T>` (or `Hash<T>`), where `T` is no `std::shared_ptr` or
//     `std::tuple` and `std::hash<T>` is defined. 
//
//     `Hash<T>{}(x)` returns `std::hash<T>{}(x)`.
//
// (2) `Hash<std::shared_ptr<T>, std::shared_ptr<T>>` (or
//     `Hash<std::shard_ptr<T>`), where `Hash<T>` is defined.
//
//     `Hash<std::shared_ptr<T>>{}(ptr)` returns `Hash<T>{}(*ptr)`.
// 
// (3) `Hash<std::shared_ptr<T>, std::shared_ptr<U>>`, where `Hash<U>` is
//     defined.
//
//     `Hash<std::shared_ptr<T>, std::shared_ptr<U>>{}(ptr)` returns
//     `Hash<U>(*std::dynamic_pointer_cast<U>(ptr))`.
//
// (4) `Hash<std::tuple<T0, ... ,Tn>, std::tuple<U0, ..., Un>>`, where
//     `Hash<Ti, Ui>` is defined for all i=0,...,n. Note: this implies
//     that either Ti, Ui are `std::shared_ptr`s or that Ti = Ui.
//
//     `Hash<std::tuple<T0, ..., Tn>, std::tuple<U0, ..., Un>>{}(t)` returns
//     `Hash<T0, U0>{}(std::get<0>(t)) ^ ... ^ Hash<Tn, Un>{}(std::get<n>(t))`.
//
// (5) `Hash<std::tuple<>, std::tuple<>>{}(t)` always returns `0`.
template<typename T, typename U = T, typename enable = void>
class Hash;  // Undefined.

// (1)
template<typename T>
class Hash<
    T,
    T,
    typename std::enable_if<
        not is_shared_ptr<T>::value and not is_tuple<T>::value
      >::type
  > : public IHash<T, T>
{
public:
  std::size_t operator()(const T& x) const override
  {
    return std::hash<T>{}(x);
  }
};

// (3) 
template<typename T, typename U>
class Hash<
    std::shared_ptr<T>,
    std::shared_ptr<U>,
    typename std::enable_if<
        std::is_base_of<T, U>::value and not std::is_same<T, U>::value
      >::type
  > : public IHash<std::shared_ptr<T>, std::shared_ptr<U>>
{
public:
  Hash(std::shared_ptr<IHash<U, U>> hash) 
  : 
    hash_{std::move(hash)}
  {}
  Hash()
  :
    Hash{std::make_shared<Hash<U, U>>()}
  {}

  std::size_t operator()(const std::shared_ptr<T>& x) const override
  {
    auto ptr = std::dynamic_pointer_cast<U>(x);
    if (not ptr)
    {
      throw std::logic_error{"Hash::operator() failed."};
    }
    return (*hash_)(*ptr);
  }

private:
  std::shared_ptr<IHash<U, U>> hash_;
};

// (2)
template<typename T>
class Hash<std::shared_ptr<T>, std::shared_ptr<T>, void>
    : public IHash<std::shared_ptr<T>, std::shared_ptr<T>>
{
public:
  Hash(std::shared_ptr<IHash<T, T>> hash) 
  : 
    hash_{std::move(hash)} 
  {}
  Hash()
  :
    Hash{std::make_shared<Hash<T, T>>()}
  {}

  std::size_t operator()(const std::shared_ptr<T>& ptr) const override
  {
    return (*hash_)(*ptr);
  }

private:
  std::shared_ptr<IHash<T, T>> hash_;
};

// (4)
template<typename T, typename U>
using IHashPointer = std::shared_ptr<IHash<T, U>>;

template<typename T, typename U>
class Hash<
    T,
    U,
    typename std::enable_if<
        is_tuple<T>::value and is_tuple<U>::value
      >::type
  > : public IHash<T, U>
{
public:
  Hash(typename Zip<T, U, IHashPointer>::type hash)
  :
    hash_{std::move(hash)}
  {}
  Hash()
  :
    Hash{make_default(std::make_index_sequence<std::tuple_size<T>::value>{})}
  {}

  std::size_t operator()(const T& t) const override
  {
    return impl(t, std::make_index_sequence<std::tuple_size<T>::value>{});
  }

private:
  // Implementation details for operator()(const T&) const.
  template<std::size_t... Is>
  std::size_t impl(const T& t, std::index_sequence<Is...>) const
  {
    return (... ^ (*std::get<Is>(hash_))(std::get<Is>(t)));
  }

  // Implementation details for Hash().
  template<std::size_t... Is>
  static typename Zip<T, U, IHashPointer>::type make_default(std::index_sequence<Is...>)
  {
    return std::make_tuple(
        std::make_shared<
            Hash<
                typename std::tuple_element<Is, T>::type,
                typename std::tuple_element<Is, U>::type
              >
          >()...
      );
  }

  typename Zip<T, U, IHashPointer>::type hash_;
};

// (5)
template<>
class Hash<std::tuple<>, std::tuple<>> : public IHash<std::tuple<>, std::tuple<>>
{
public:
  std::size_t operator()(const std::tuple<>& t) const override
  {
    return 0;
  }
};

}} // namespace drmock

#endif /* DRMOCK_SRC_MOCK_DETAIL_HASH_H */
