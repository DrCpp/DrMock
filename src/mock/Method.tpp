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

#include "detail/Diagnostics.h"

namespace drmock {

template<typename Result, typename... Args>
Method<Result, Args...>::Method()
:
  Method{""}
{}

template<typename Result, typename... Args>
Method<Result, Args...>::Method(std::string name)
:
  Method{std::move(name), std::make_shared<StateObject>()}
{}

template<typename Result, typename... Args>
Method<Result, Args...>::Method(std::shared_ptr<StateObject> state_object)
:
  Method{"", std::move(state_object)}
{}

template<typename Result, typename... Args>
Method<Result, Args...>::Method(std::string name, std::shared_ptr<StateObject> state_object)
:
  name_{std::move(name)},
  is_tuple_pack_equal_{std::make_shared<detail::IsTuplePackEqual<std::tuple<Args...>>>()},
  state_object_{std::move(state_object)},
  state_behavior_{},
  behavior_stack_{std::make_shared<BehaviorStack<Result, Args...>>(is_tuple_pack_equal_)},
  behavior_{behavior_stack_}
{}

template<typename Result, typename... Args>
BehaviorStack<Result, Args...>&
Method<Result, Args...>::io()
{
  if (not behavior_stack_)
  {
    behavior_stack_ = std::make_shared<BehaviorStack<Result, Args...>>(is_tuple_pack_equal_);
  }
  behavior_ = behavior_stack_;
  return *behavior_stack_;
}

template<typename Result, typename... Args>
Behavior<Result, Args...>&
Method<Result, Args...>::push()
{
  return io().push();
}

template<typename Result, typename... Args>
void
Method<Result, Args...>::enforce_order(bool value)
{
  io().enforce_order(value);
}

template<typename Result, typename... Args>
StateBehavior<Result, Args...>&
Method<Result, Args...>::state()
{
  if (not state_behavior_)
  {
    state_behavior_ = std::make_shared<StateBehavior<Result, Args...>>(
        state_object_,
        is_tuple_pack_equal_
      );
  }
  behavior_ = state_behavior_;
  return *state_behavior_;
}

template<typename Result, typename... Args>
template<typename... Deriveds>
void
Method<Result, Args...>::polymorphic()
{
  is_tuple_pack_equal_ = std::make_shared<detail::IsTuplePackEqual<
      std::tuple<Args...>,
      std::tuple<Deriveds...>
    >>();
  if (behavior_stack_)
  {
    behavior_stack_->setIsEqual(is_tuple_pack_equal_);
  }
  if (state_behavior_)
  {
    state_behavior_->setIsEqual(is_tuple_pack_equal_);
  }
}

template<typename Result, typename... Args>
bool
Method<Result, Args...>::verify() const
{
  // If behavior_stack_ is used for verification, check if it's
  // exhausted.
  if (not state_behavior_)
  {
    return (not has_failed_) and behavior_stack_->is_exhausted();
  }
  return not has_failed_;
}

template<typename Result, typename... Args>
const std::vector<std::vector<std::string>>&
Method<Result, Args...>::error_msgs() const
{
  return error_msgs_;
}

template<typename Result, typename... Args>
std::shared_ptr<typename std::decay<Result>::type>
Method<Result, Args...>::call(const Args&... args)
{
  auto result = behavior_->call(args...);
  if (std::holds_alternative<std::exception_ptr>(result))
  {
    std::rethrow_exception(std::get<std::exception_ptr>(result));
  }
  else if (std::holds_alternative<std::shared_ptr<DecayedResult>>(result))
  {
    auto rv = std::get<std::shared_ptr<DecayedResult>>(result);
    if (rv or std::is_same_v<DecayedResult, void>)
    {
      return rv;
    }
  }

  has_failed_ = true;
  error_msgs_.push_back({});
  detail::PrintAll<Args...>{}(error_msgs_.back(), args...);

  if constexpr (std::is_default_constructible_v<DecayedResult>)
  {
    return std::make_shared<DecayedResult>();
  }
  else if constexpr(std::is_same_v<DecayedResult, void>)
  {
    return {};
  }
  else
  {
    std::abort();
  }
}

template<typename Result, typename... Args>
std::string
Method<Result, Args...>::makeFormatedErrorString() const
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

} // namespace drmock
