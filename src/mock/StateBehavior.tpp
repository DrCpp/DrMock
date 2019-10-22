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

namespace drmock {

template<typename Result, typename... Args>
StateBehavior<Result, Args...>::StateBehavior()
:
  StateBehavior{
      std::make_shared<StateObject>(),
      std::make_shared<detail::IsTuplePackEqual<std::tuple<Args...>>>()
    }
{}

template<typename Result, typename... Args>
StateBehavior<Result, Args...>::StateBehavior(
    std::shared_ptr<StateObject> state_object
  )
:
  StateBehavior{
      state_object,
      std::make_shared<detail::IsTuplePackEqual<std::tuple<Args...>>>()
    }
{}

template<typename Result, typename... Args>
StateBehavior<Result, Args...>::StateBehavior(
    std::shared_ptr<StateObject> state_object,
    std::shared_ptr<detail::IIsTuplePackEqual<Args...>> is_tuple_pack_equal
  )
:
  state_object_{state_object},
  is_tuple_pack_equal_{is_tuple_pack_equal}
{}

template<typename Result, typename... Args>
StateBehavior<Result, Args...>&
StateBehavior<Result, Args...>::transition(
    std::string current_state,
    std::string new_state,
    Args... input
  )
{
  transition(
      "",
      std::move(current_state),
      std::move(new_state),
      std::move(input)...
    );
  return *this;
}

template<typename Result, typename... Args>
StateBehavior<Result, Args...>&
StateBehavior<Result, Args...>::transition(
    const std::string slot,
    std::string current_state,
    std::string new_state,
    Args... input
  )
{
  // Register the slot in the `StateObject`.
  state_object_->get(slot);
  // Get the transitions for this slot.
  auto& map = transitions_[slot];
  // Check if a (wildcard) transition matching the key exists.
  auto key = std::make_tuple(current_state, std::make_tuple(input...));
  if (current_state != "*")
  {
    auto wildkey = std::make_tuple("*", std::make_tuple(std::move(input)...));
    if (map.find(key) != map.end() or map.find(wildkey) != map.end())
    {
      throw std::runtime_error{"Transition conflict."};
    }
  }
  else
  {
    // Check *all* transitions and compare their input to `input`.
    for (const auto& v : map)
    {
      if ((*is_tuple_pack_equal_)(std::get<1>(v.first), input...))
      {
        throw std::runtime_error{"Transition conflict."};
      }
    }
  }
  // If all checks out, add the transition.
  map[key] = std::move(new_state);
  return *this;
}

template<typename Result, typename... Args>
template<typename... Deriveds>
StateBehavior<Result, Args...>&
StateBehavior<Result, Args...>::polymorphic()
{
  is_tuple_pack_equal_ = std::make_shared<detail::IsTuplePackEqual<
    std::tuple<Args...>,
    std::tuple<Deriveds...>
  >>();
  return *this;
}

template<typename Result, typename... Args>
void
StateBehavior<Result, Args...>::setIsEqual(
    std::shared_ptr<detail::IIsTuplePackEqual<Args...>> is_tuple_pack_equal
  )
{
  is_tuple_pack_equal_ = std::move(is_tuple_pack_equal);
}

template<typename Result, typename... Args>
template<typename T>
StateBehavior<Result, Args...>&
StateBehavior<Result, Args...>::returns(
    const std::string& state,
    const std::enable_if_t<not std::is_same_v<Result, void>, T>& value
  )
{
  returns("", state, value);
  return *this;
}

template<typename Result, typename... Args>
template<typename T>
StateBehavior<Result, Args...>&
StateBehavior<Result, Args...>::returns(
    const std::string& state,
    std::enable_if_t<not std::is_same_v<Result, void>, T>&& value
  )
{
  returns("", state, std::move(value));
  return *this;
}

template<typename Result, typename... Args>
template<typename T>
StateBehavior<Result, Args...>&
StateBehavior<Result, Args...>::returns(
    const std::string& slot,
    const std::string& state,
    const std::enable_if_t<not std::is_same_v<Result, void>, T>& value
  )
{
  setResultSlot(slot);
  throwOnConflict(slot, state);
  auto ptr = std::make_shared<Result>(value);
  results_[state] = std::move(ptr);
  return *this;
}

template<typename Result, typename... Args>
template<typename T>
StateBehavior<Result, Args...>&
StateBehavior<Result, Args...>::returns(
    const std::string& slot,
    const std::string& state,
    std::enable_if_t<not std::is_same_v<Result, void>, T>&& value
  )
{
  setResultSlot(slot);
  throwOnConflict(slot, state);
  auto ptr = std::make_shared<Result>(value);
  results_[state] = std::move(ptr);
  return *this;
}

template<typename Result, typename... Args>
template<typename E>
StateBehavior<Result, Args...>&
StateBehavior<Result, Args...>::throws(
    const std::string& state,
    E&& excp
  )
{
  throws("", state, excp);
  return *this;
}

template<typename Result, typename... Args>
template<typename E>
StateBehavior<Result, Args...>&
StateBehavior<Result, Args...>::throws(
    const std::string& slot,
    const std::string& state,
    E&& excp
  )
{
  setResultSlot(slot);
  throwOnConflict(slot, state);
  auto ptr = std::make_exception_ptr(excp);
  results_[state] = std::move(ptr);
  return *this;
}

template<typename Result, typename... Args>
std::variant<
    std::monostate,
    std::shared_ptr<typename std::decay<Result>::type>,
    std::exception_ptr
  >
StateBehavior<Result, Args...>::call(const Args&... args)
{
  // Transition all slots.
  for (const auto& v : transitions_)
  {
    std::string slot = v.first;
    auto& map = v.second;
    // Find the first transition that matches the arguments.
    for (const auto& w : map)
    {
      std::string current_state = state_object_->get(slot);
      auto& key = w.first;
      if (
              (std::get<0>(key) == current_state or std::get<0>(key) == "*")
          and (*is_tuple_pack_equal_)(std::get<1>(key), args...)
      )
      {
        std::string new_state = w.second;
        state_object_->set(slot, new_state);
        break;
      }
    }
  }
  // Get the slot's state.
  auto state = state_object_->get(slot_);
  // Return the result if possible.
  if (results_.find(state) != results_.end())
  {
    return results_[state];
  }
  // If no result was found, but the function is void, return void.
  if constexpr (std::is_same<typename std::decay<Result>::type, void>::value)
  {
    return std::shared_ptr<void>{};
  }
  return std::monostate{};
}

template<typename Result, typename... Args>
void
StateBehavior<Result, Args...>::setResultSlot(const std::string& slot)
{
  if (not fixed_)
  {
    slot_ = slot;
    fixed_ = true;
  }
}

template<typename Result, typename... Args>
void
StateBehavior<Result, Args...>::throwOnConflict(
    const std::string& slot,
    const std::string& state
  ) const
{
  if (fixed_ and slot != slot_)
  {
    throw std::runtime_error{"Result slot already set to '" + slot_ + "'."};
  }
  else if (results_.find(state) != results_.end())
  {
    throw std::runtime_error{"Result already defined for state '" + state + "'."};
  }
}

} // namespace drmock
