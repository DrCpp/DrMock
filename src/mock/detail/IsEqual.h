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

/* IsEqual

Class template with `operator()` that compares two objects by the
following recursive rules:

(1) If `T` is no shared pointer, unique pointer or raw pointer and
`T::operator==(const T&) const` is defined, then

  `IsEqual<T>{}(x, y)` returns `x == y`.

(2) If `IsEqual<T>` is defined, then

  `IsEqual<T*>{}(x, y)` returns `IsEqual<T>{}(*x, *y)`.

(Similar for shared pointers and unique pointers.)

(3) If `IsEqual<U>` is defined, then

  `IsEqual<T*, U*>{}(x, y)` returns

  `IsEqual<U, U>(*std::dynamic_pointer_cast<U>(x),
                 *std::dynamic_pointer_cast<U>(y))`.

(4) If T, U are same-size tuples and `IsEqual<Ti, Ui>` is defined for
all i = 0 .. n, then

  `IsEqual<T, U>{}(t, u)` returns

     `IsEqual<T0, U0>{}(std::get<0>(t), std::get<0>(u))
  and IsEqual<T1, U1>{}(std::get<1>(t), std::get<1>(u))
  and ...
  and IsEqual<T1, U1>{}(std::get<n>(t), std::get<n>(u))`.

(4') If T, U are `std::tuple<>` then `IsEqual<T, U>::operator()` always
returns zero.

Otherwise, IsEqual is undefined. Note that the specializations may not
be implemented in numerical order.
*/

template<typename T, typename U = T, typename enable = void>
struct IsEqual;  // Undefined.

/* ************************************
 * (1)
 * ************************************ */
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


/* ************************************
 * (3)
 * ************************************ */
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
  bool operator()(const T& lhs, const T& rhs) const override
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
  bool operator()(const T& lhs, const T& rhs) const override
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

/* ************************************
 * (2)
 * ************************************ */
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
  bool operator()(const T& lhs, const T& rhs) const override
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
  bool operator()(const T& lhs, const T& rhs) const override
  {
    return IsEqual<typename std::remove_pointer<T>::type>{}(*lhs, *rhs);
  }
};

/* ************************************
 * (4)
 * ************************************ */
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

}} // namespace drmock

#endif /* DRMOCK_SRC_MOCK_DETAIL_ISEQUAL_H */
