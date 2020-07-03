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

#include "detail/IsTuplePackEqual.h"
#include "Signal.h"

namespace drmock {

template<typename Class, typename ReturnType, typename... Args>
Behavior<Class, ReturnType, Args...>::Behavior()
:
  Behavior{std::make_shared<detail::IsTuplePackEqual<std::tuple<Args...>>>()}
{}

template<typename Class, typename ReturnType, typename... Args>
Behavior<Class, ReturnType, Args...>::Behavior(
    std::shared_ptr<detail::IIsTuplePackEqual<Args...>> is_tuple_pack_equal
  )
:
  is_tuple_pack_equal_{std::move(is_tuple_pack_equal)}
{}

template<typename Class, typename ReturnType, typename... Args>
template<typename T>
std::enable_if_t<(std::tuple_size_v<T> > 0), Behavior<Class, ReturnType, Args...>&>
Behavior<Class, ReturnType, Args...>::expects()
{
  expect_.reset();
  return *this;
}

template<typename Class, typename ReturnType, typename... Args>
Behavior<Class, ReturnType, Args...>&
Behavior<Class, ReturnType, Args...>::expects(Args... args)
{
  if (expect_.has_value())
  {
    throw std::runtime_error{
        "Behavior object already configured. Please check your mock object configuration."
      };
  }
  expect_.emplace(std::move(args)...);
  return *this;
}

template<typename Class, typename ReturnType, typename... Args>
template<typename T>
Behavior<Class, ReturnType, Args...>&
Behavior<Class, ReturnType, Args...>::returns(T&& result)
{
  if (result_.first or exception_)
  {
    throw std::runtime_error{
        "Behavior object already configured. Please check your mock object configuration."
      };
  }
  result_.first = std::make_shared<std::decay_t<ReturnType>>(std::forward<T>(result));
  return *this;
}

template<typename Class, typename ReturnType, typename... Args>
template<typename E>
Behavior<Class, ReturnType, Args...>&
Behavior<Class, ReturnType, Args...>::throws(E&& exception)
{
  if (result_.first or result_.second or exception_)
  {
    throw std::runtime_error{
        "Behavior object already configured. Please check your mock object configuration."
      };
  }
  exception_ = std::make_exception_ptr(std::forward<E>(exception));
  return *this;
}

template<typename Class, typename ReturnType, typename... Args>
template<typename... SignalArgs>
Behavior<Class, ReturnType, Args...>&
Behavior<Class, ReturnType, Args...>::emits(void (Class::*signal)(SignalArgs...), SignalArgs&&... args)
{
  if (result_.second or exception_)
  {
    throw std::runtime_error{
        "Behavior object already configured. Please check your mock object configuration."
      };
  }
  result_.second = std::make_shared<Signal<Class, SignalArgs...>>(
      signal,
      std::forward<SignalArgs>(args)...
    );
  return *this;
}

template<typename Class, typename ReturnType, typename... Args>
Behavior<Class, ReturnType, Args...>&
Behavior<Class, ReturnType, Args...>::times(unsigned int t)
{
  times(t, t);
  return *this;
}

template<typename Class, typename ReturnType, typename... Args>
Behavior<Class, ReturnType, Args...>&
Behavior<Class, ReturnType, Args...>::times(
    unsigned int times_min,
    unsigned int times_max
  )
{
  static_assert(
      std::is_reference_v<ReturnType>
        or std::is_pointer_v<ReturnType>
        or std::is_copy_constructible_v<ReturnType>
        or std::is_same_v<ReturnType, void>,
      "result type must be reference, pointer or copy-constructible to set times()"
    );
  if (times_min > times_max)
  {
    throw std::runtime_error{"minimum number of expected calls must be less or equal to maximum number of expected calls"};
  }
  times_min_ = times_min;
  times_max_ = times_max;
  return *this;
}

template<typename Class, typename ReturnType, typename... Args>
Behavior<Class, ReturnType, Args...>&
Behavior<Class, ReturnType, Args...>::persists()
{
  static_assert(
      std::is_reference_v<ReturnType>
        or std::is_pointer_v<ReturnType>
        or std::is_copy_constructible_v<ReturnType>
        or std::is_same_v<ReturnType, void>,
      "result type must be reference, pointer or copy-constructible to set persists()"
    );
  persists_ = true;
  return *this;
}

template<typename Class, typename ReturnType, typename... Args>
bool
Behavior<Class, ReturnType, Args...>::is_persistent() const
{
  return persists_ or (num_calls_ < times_max_);
}

template<typename Class, typename ReturnType, typename... Args>
bool
Behavior<Class, ReturnType, Args...>::is_exhausted() const
{
  return persists_ or ((times_min_ <= num_calls_) and (num_calls_ <= times_max_));
}

template<typename Class, typename ReturnType, typename... Args>
template<typename... Deriveds>
Behavior<Class, ReturnType, Args...>&
Behavior<Class, ReturnType, Args...>::polymorphic()
{
  is_tuple_pack_equal_ = std::make_shared<detail::IsTuplePackEqual<
      std::tuple<Args...>,
      std::tuple<Deriveds...>
    >>();
  return *this;
}

template<typename Class, typename ReturnType, typename... Args>
bool
Behavior<Class, ReturnType, Args...>::match(const Args&... args) const
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

template<typename Class, typename ReturnType, typename... Args>
std::variant<
    typename Behavior<Class, ReturnType, Args...>::Result,
    std::exception_ptr
  >
Behavior<Class, ReturnType, Args...>::produce()
{
  if (num_calls_ < times_max_)
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

template<typename Class, typename ReturnType, typename... Args>
void
Behavior<Class, ReturnType, Args...>::setIsEqual(
    std::shared_ptr<detail::IIsTuplePackEqual<Args...>> is_tuple_pack_equal
  )
{
  is_tuple_pack_equal_ = std::move(is_tuple_pack_equal);
}

} // namespace
