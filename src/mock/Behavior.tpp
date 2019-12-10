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

#include <iostream>  // TODO Remove this!

#include "detail/IsTuplePackEqual.h"

namespace drmock {

template<typename Result, typename... Args>
Behavior<Result, Args...>::Behavior()
:
  Behavior{std::make_shared<detail::IsTuplePackEqual<std::tuple<Args...>>>()}
{}

template<typename Result, typename... Args>
Behavior<Result, Args...>::Behavior(
    std::shared_ptr<detail::IIsTuplePackEqual<Args...>> is_tuple_pack_equal
  )
:
  is_tuple_pack_equal_{std::move(is_tuple_pack_equal)}
{}

template<typename Result, typename... Args>
template<typename T>
typename std::enable_if<(std::tuple_size<T>::value > 0), Behavior<Result, Args...>&>::type
Behavior<Result, Args...>::expects()
{
  expect_.reset();
  return *this;
}

template<typename Result, typename... Args>
Behavior<Result, Args...>&
Behavior<Result, Args...>::expects(Args... args)
{
  expect_.emplace(std::move(args)...);
  return *this;
}

template<typename Result, typename... Args>
template<typename T>
Behavior<Result, Args...>&
Behavior<Result, Args...>::returns(T&& result)
{
  result_ = std::make_shared<typename std::decay<Result>::type>(std::forward<T>(result));
  return *this;
}

template<typename Result, typename... Args>
template<typename E>
Behavior<Result, Args...>&
Behavior<Result, Args...>::throws(E&& exception)
{
  exception_ = std::make_exception_ptr(std::forward<E>(exception));
  return *this;
}

template<typename Result, typename... Args>
Behavior<Result, Args...>&
Behavior<Result, Args...>::times(unsigned int times)
{
  static_assert(
      std::is_reference_v<Result>
        or std::is_pointer_v<Result>
        or std::is_copy_constructible_v<Result>
        or std::is_same_v<Result, void>,
      "result type must be reference, pointer or copy-constructible to set times()"
    );
  times_ = times;
  return *this;
}

template<typename Result, typename... Args>
Behavior<Result, Args...>&
Behavior<Result, Args...>::persists()
{
  static_assert(
      std::is_reference_v<Result>
        or std::is_pointer_v<Result>
        or std::is_copy_constructible_v<Result>
        or std::is_same_v<Result, void>,
      "result type must be reference, pointer or copy-constructible to set persists()"
    );
  persists_ = true;
  return *this;
}

template<typename Result, typename... Args>
bool
Behavior<Result, Args...>::is_persistent() const
{
  return persists_ or (num_calls_ < times_);
}

template<typename Result, typename... Args>
bool
Behavior<Result, Args...>::is_exhausted() const
{
  return persists_ or (num_calls_ == times_);
}

template<typename Result, typename... Args>
template<typename... Deriveds>
Behavior<Result, Args...>&
Behavior<Result, Args...>::polymorphic()
{
  is_tuple_pack_equal_ = std::make_shared<detail::IsTuplePackEqual<
      std::tuple<Args...>,
      std::tuple<Deriveds...>
    >>();
  return *this;
}

template<typename Result, typename... Args>
bool
Behavior<Result, Args...>::match(const Args&... args) const
{
  if (expect_)
  {
    return (*is_tuple_pack_equal_)(*expect_, args...);
  }
  else
  {
    return true;
  }
}

template<typename Result, typename... Args>
std::variant<
    std::shared_ptr<typename std::decay<Result>::type>,
    std::exception_ptr
  >
Behavior<Result, Args...>::produce()
{
  if (num_calls_ < times_)
  {
    ++num_calls_;
  }

  if (exception_)
  {
    return exception_;
  }
  else
  {
    return result_;
  }
}

template<typename Result, typename... Args>
void
Behavior<Result, Args...>::setIsEqual(
    std::shared_ptr<detail::IIsTuplePackEqual<Args...>> is_tuple_pack_equal
  )
{
  is_tuple_pack_equal_ = std::move(is_tuple_pack_equal);
}

} // namespace
