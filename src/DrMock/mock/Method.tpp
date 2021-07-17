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

#include <DrMock/utility/detail/Diagnostics.h>

namespace drmock {

template<typename Class, typename ReturnType, typename... Args>
Method<Class, ReturnType, Args...>::Method()
:
  Method{""}
{}

template<typename Class, typename ReturnType, typename... Args>
Method<Class, ReturnType, Args...>::Method(std::string name)
:
  Method{std::move(name), std::make_shared<StateObject>()}
{}

template<typename Class, typename ReturnType, typename... Args>
Method<Class, ReturnType, Args...>::Method(std::shared_ptr<StateObject> state_object)
:
  Method{"", std::move(state_object)}
{}

template<typename Class, typename ReturnType, typename... Args>
Method<Class, ReturnType, Args...>::Method(std::string name, std::shared_ptr<StateObject> state_object)
:
  name_{std::move(name)},
  make_tuple_of_matchers_{std::make_shared<detail::MakeTupleOfMatchers<std::tuple<Args...>>>()},
  state_object_{std::move(state_object)},
  state_behavior_{},
  behavior_queue_{std::make_shared<BehaviorQueue<Class, ReturnType, Args...>>(make_tuple_of_matchers_)},
  behavior_{behavior_queue_}
{}

template<typename Class, typename ReturnType, typename... Args>
BehaviorQueue<Class, ReturnType, Args...>&
Method<Class, ReturnType, Args...>::io()
{
  if (not behavior_queue_)
  {
    behavior_queue_ = std::make_shared<BehaviorQueue<Class, ReturnType, Args...>>(make_tuple_of_matchers_);
  }
  behavior_ = behavior_queue_;
  return *behavior_queue_;
}

template<typename Class, typename ReturnType, typename... Args>
Behavior<Class, ReturnType, Args...>&
Method<Class, ReturnType, Args...>::push()
{
  return io().push();
}

template<typename Class, typename ReturnType, typename... Args>
void
Method<Class, ReturnType, Args...>::enforce_order(bool value)
{
  io().enforce_order(value);
}

template<typename Class, typename ReturnType, typename... Args>
StateBehavior<Class, ReturnType, Args...>&
Method<Class, ReturnType, Args...>::state()
{
  if (not state_behavior_)
  {
    state_behavior_ = std::make_shared<StateBehavior<Class, ReturnType, Args...>>(
        state_object_,
        make_tuple_of_matchers_
      );
  }
  behavior_ = state_behavior_;
  return *state_behavior_;
}

template<typename Class, typename ReturnType, typename... Args>
template<typename... Deriveds>
void
Method<Class, ReturnType, Args...>::polymorphic()
{
  make_tuple_of_matchers_ = std::make_shared<detail::MakeTupleOfMatchers<
      std::tuple<Args...>,
      std::tuple<Deriveds...>
    >>();
  if (behavior_queue_)
  {
    behavior_queue_->template polymorphic<Deriveds...>();
  }
  if (state_behavior_)
  {
    state_behavior_->template polymorphic<Deriveds...>();
  }
}

template<typename Class, typename ReturnType, typename... Args>
bool
Method<Class, ReturnType, Args...>::verify() const
{
  // If behavior_queue_ is used for verification, check if it's
  // exhausted.
  if (not state_behavior_)
  {
    return (not has_failed_) and behavior_queue_->is_exhausted();
  }
  return not has_failed_;
}

template<typename Class, typename ReturnType, typename... Args>
const std::vector<std::vector<std::string>>&
Method<Class, ReturnType, Args...>::error_msgs() const
{
  return error_msgs_;
}

template<typename Class, typename ReturnType, typename... Args>
std::shared_ptr<typename std::decay<ReturnType>::type>
Method<Class, ReturnType, Args...>::call(const Args&... args)
{
  auto result = behavior_->call(args...);
  if (std::holds_alternative<std::exception_ptr>(result))
  {
    std::rethrow_exception(std::get<std::exception_ptr>(result));
  }
  else if (std::holds_alternative<std::pair<std::shared_ptr<typename std::decay<ReturnType>::type>, std::shared_ptr<AbstractSignal<Class>>>>(result))
  {
    auto p = std::get<std::pair<std::shared_ptr<typename std::decay<ReturnType>::type>, std::shared_ptr<AbstractSignal<Class>>>>(result);
    auto rv = p.first;
    if (rv or std::is_same_v<DecayedReturnType, void>)
    {
      auto signal_name = p.second;
      if (signal_name)
      {
        signal_name->invoke(parent_);
      }
      return rv;
    }
  }

  has_failed_ = true;
  error_msgs_.push_back({});
  drutility::detail::PrintAll<Args...>{}(error_msgs_.back(), args...);

  if constexpr (std::is_default_constructible_v<DecayedReturnType>)
  {
    return std::make_shared<DecayedReturnType>();
  }
  else if constexpr(std::is_same_v<DecayedReturnType, void>)
  {
    return {};
  }
  else
  {
    std::abort();
  }
}

template<typename Class, typename ReturnType, typename... Args>
std::string
Method<Class, ReturnType, Args...>::makeFormattedErrorString() const
{
  std::stringstream s{};
  s << std::endl << "  Method \"" << name_ << "\" failed because" << std::endl;
  for (auto& outer : error_msgs_)
  {
    s << "  " << "  " << "no behavior specified for call" << std::endl;
    for (auto& inner : outer)
    {
      s << "  " << "  " << "  " << inner << std::endl;
    }
  }
  return s.str();
}

template<typename Class, typename ReturnType, typename... Args>
void
Method<Class, ReturnType, Args...>::parent(Class* parent)
{
  parent_ = parent;
}

} // namespace drmock
