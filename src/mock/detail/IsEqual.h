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

#ifndef DRMOCK_SRC_MOCK_DETAIL_ISEQUAL_H
#define DRMOCK_SRC_MOCK_DETAIL_ISEQUAL_H

#include <memory>
#include <type_traits>

#include "IIsEqual.h"

#include "TypeTraits.h"

namespace drmock { namespace detail {

// A recursively defined template struct similar to `std::equal_to`.
//
// (1) `IsEqual<T, T>`, where `T` is no `std::shared_ptr` or
//     `std::tuple` and `T::operator==(const T&)` is defined.
//
//     `IsEqual<T>{}(x, y)` returns `x == y`.
//
// (2) `IsEqual<std::shared_ptr<T>, std::shared_ptr<T>>` (or
//     `IsEqual<std::shared_ptr<T>>`), where `IsEqual<T, T>` is defined.
//
//     `IsEqual<std::shared_ptr<T>>{}(p, q)` returns `IsEqual<T>(*p, *q)`.
//
// (3) `IsEqual<std::shared_ptr<T>, std::shared_ptr<U>>`, where
//     `IsEqual<U>` is defined.
//
//     `IsEqual<std::shared_ptr<T>, std::shared_ptr<U>>{}(p, q)` returns
//     `IsEqual<U, U>(*std::dynamic_pointer_cast<U>(p), *std::dynamic_pointer_cast<U>(q))`.
//
// (4) `IsEqual<std::tuple<T0, ... ,Tn>, std::tuple<U0, ..., Un>>`,
//     where `IsEqual<Ti, Ui>` is defined for all i=0,...,n. Note: this
//     implies that either Ti, Ui are `std::shared_ptr`s or that Ti = Ui.
//
//     `IsEqual<std::tuple<T0, ..., Tn>, std::tuple<U0, ..., Un>>{}(t, u)`
//     returns
//     `IsEqual<T0, U0>{}(std::get<0>(t), std::get<0>(u)) and ...
//     and IsEqual<Tn, Un>{}(std::get<n>(t), std::get<n>(u))`.
//
// (5) `IsEqual<std::tuple<>, std::tuple<>>{}(t, u)` always returns
//     `true`.
template<typename T, typename U = T, typename enable = void>
struct IsEqual;  // Undefined.

//  (1) `IsEqual` for comparing objects that are not `std::shared_ptr`s
//      or `std::tuple`s. `IsEqual::invoke(x, y)` returns `x == y`.
template<typename T>
struct IsEqual<
    T,
    T,
    typename std::enable_if<
            not is_shared_ptr<T>::value
        and not is_unique_ptr<T>::value
        and not is_tuple<T>::value
        and not std::is_pointer<T>::value
      >::type
  > : public IIsEqual<T>
{
  bool operator()(const T& lhs, const T& rhs) const override
  {
    if constexpr(std::is_abstract_v<T>)
    {
      throw std::logic_error{"Cannot compare abstract type"};
    }
    else
    {
      return (lhs == rhs);
    }
  }
};

//  (3) IsEqual for comparing `std::shared_ptr<T>`s and `std::shared_ptr<U>`s as
//  well as `std::unique_ptr<T>`s and `std::unique_ptr<U>`s where `U` is derived
//  from `T`. `IsEqual::invoke(p, q)` will return `true` if `p` and `q` point to
//  objects of type `U` that are equal, `false` otherwise.
template<typename T, typename U>
struct IsEqual<
    T,
    U,
    typename std::enable_if<
            is_shared_ptr<T>::value
        and is_shared_ptr<U>::value
        and std::is_base_of<typename T::element_type, typename U::element_type>::value 
        and not std::is_same<T, U>::value
      >::type
  > : public IIsEqual<T>
{
  bool operator()(
      const T& lhs,
      const T& rhs
    ) const override
  {
    using ElemType = typename U::element_type;
    std::shared_ptr<ElemType> lhs_cast = std::dynamic_pointer_cast<ElemType>(lhs);
    std::shared_ptr<ElemType> rhs_cast = std::dynamic_pointer_cast<ElemType>(rhs);
    return (
        lhs_cast and rhs_cast and IsEqual<ElemType>{}(*lhs_cast, *rhs_cast)
      );
  }
};

template<typename T, typename U>
struct IsEqual<
    T,
    U,
    typename std::enable_if<
            is_unique_ptr<T>::value
        and is_unique_ptr<U>::value
        and std::is_base_of<typename T::element_type, typename U::element_type>::value
        and not std::is_same<T, U>::value
      >::type
  > : public IIsEqual<T>
{
  bool operator()(
      const T& lhs,
      const T& rhs
    ) const override
  {
    // Cannot cast a unique_ptr, need to cast the raw pointer and wrap it in
    // shared_ptr with noop deleter.
    using ElemType = typename U::element_type;
    std::shared_ptr<ElemType> lhs_cast{dynamic_cast<ElemType*>(lhs.get()), [] (auto) {}};
    std::shared_ptr<ElemType> rhs_cast{dynamic_cast<ElemType*>(rhs.get()), [] (auto) {}};
    return (
        lhs_cast and rhs_cast and IsEqual<ElemType>{}(*lhs_cast, *rhs_cast)
      );
  }
};

//  (2) Comparison of `std::shared_ptr`s and `std::unique_ptr`s of the same
//  type. `IsEqual::invoke(p, q)` returns `true` if the objects pointed to by
//  `p` and `q` are equal, `false` otherwise.
template<typename T>
struct IsEqual<
    T,
    T,
    typename std::enable_if<
            is_shared_ptr<T>::value
         or is_unique_ptr<T>::value
      >::type
  >
    : public IIsEqual<T>
{
  bool operator()(
      const T& lhs,
      const T& rhs
    ) const override
  {
    using ElemType = typename T::element_type;
    return IsEqual<ElemType>{}(*lhs, *rhs);
  }
};

template<typename T>
struct IsEqual<
    T,
    T,
    typename std::enable_if<
            std::is_pointer<T>::value
      >::type
  >
    : public IIsEqual<T>
{
  bool operator()(
      const T& lhs,
      const T& rhs
    ) const override
  {
    return IsEqual<typename std::remove_pointer<T>::type>{}(*lhs, *rhs);
  }
};

// (4)
template<typename T, typename U>
struct IsEqual<
    T,
    U,
    typename std::enable_if<
        is_tuple<T>::value and is_tuple<U>::value
        and (std::tuple_size<T>::value == std::tuple_size<U>::value)
      >::type
  > : public IIsEqual<T>
{
public:
  bool operator()(const T& lhs, const T& rhs) const override
  {
    return impl(
        lhs,
        rhs,
        std::make_index_sequence<std::tuple_size<T>::value>{}
      );
  }

private:
  // Implementation detail of `operator()(const T&, const T&)`.
  template<std::size_t... Is>
  static bool impl(const T& lhs, const T& rhs, std::index_sequence<Is...>)
  {
    return (... and IsEqual<
            typename std::tuple_element<Is, T>::type,
            typename std::tuple_element<Is, U>::type
          >{}(std::get<Is>(lhs), std::get<Is>(rhs))
      );
  }
};

// (5) Specialization for the empty tuple; `operator()(...)` always returns
// `true`.
template<>
struct IsEqual<std::tuple<>, std::tuple<>> : public IIsEqual<std::tuple<>>
{
  bool operator()(const std::tuple<>&, const std::tuple<>&) const override
  {
    return true;
  }
};

template<typename T, typename U>
bool is_equal(const T& lhs, const U& rhs)
{
  return IsEqual<T, U>{}.operator()(lhs, rhs);
}

}} // namespace drmock

#endif /* DRMOCK_SRC_MOCK_DETAIL_ISEQUAL_H */
