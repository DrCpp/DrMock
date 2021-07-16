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

#include <DrMock/mock/detail/WrapInSharedEqual.h>
#include <DrMock/mock/Signal.h>

namespace drmock {

template<typename Class, typename ReturnType, typename... Args>
StateBehavior<Class, ReturnType, Args...>::StateBehavior()
:
  StateBehavior{
      std::make_shared<StateObject>(),
      std::make_shared<detail::WrapInSharedEqual<std::tuple<Args...>>>()
    }
{}

template<typename Class, typename ReturnType, typename... Args>
StateBehavior<Class, ReturnType, Args...>::StateBehavior(
    std::shared_ptr<StateObject> state_object
  )
:
  StateBehavior{
      state_object,
      std::make_shared<detail::WrapInSharedEqual<std::tuple<Args...>>>()
    }
{}

template<typename Class, typename ReturnType, typename... Args>
StateBehavior<Class, ReturnType, Args...>::StateBehavior(
    std::shared_ptr<StateObject> state_object,
    std::shared_ptr<detail::IWrapInSharedEqual<Args...>> wrap_in_shared_equal
  )
:
  state_object_{state_object},
  wrap_in_shared_equal_{wrap_in_shared_equal}
{}

template<typename Class, typename ReturnType, typename... Args>
StateBehavior<Class, ReturnType, Args...>&
StateBehavior<Class, ReturnType, Args...>::transition(
    const std::string& current_state,
    std::string new_state,
    Args... input
  )
{
  return transition(
      wrap_in_shared_equal_,
      "",
      current_state,
      std::move(new_state),
      std::move(input)...
    );
}

template<typename Class, typename ReturnType, typename... Args>
StateBehavior<Class, ReturnType, Args...>&
StateBehavior<Class, ReturnType, Args...>::transition(
    const std::string& slot,
    const std::string& current_state,
    std::string new_state,
    Args... input
  )
{
  return transition(
      wrap_in_shared_equal_,
      slot,
      current_state,
      std::move(new_state),
      std::move(input)...
    );
}

template<typename Class, typename ReturnType, typename... Args>
template<typename... Deriveds>
StateBehavior<Class, ReturnType, Args...>&
StateBehavior<Class, ReturnType, Args...>::transition(
    const std::string& current_state,
    std::string new_state,
    Args... input
  )
{
  return transition<Deriveds...>(
      "",
      current_state,
      std::move(new_state),
      std::move(input)...
    );
}

template<typename Class, typename ReturnType, typename... Args>
template<typename... Deriveds>
StateBehavior<Class, ReturnType, Args...>&
StateBehavior<Class, ReturnType, Args...>::transition(
    const std::string& slot,
    const std::string& current_state,
    std::string new_state,
    Args... input
  )
{
  static_assert(
      drutility::detail::is_base_of_tuple_v<std::tuple<std::decay_t<Args>...>, std::tuple<std::decay_t<Deriveds>...>>,
      "Specified impossible polymorphic setting"
    );
  return transition(
      std::make_shared<detail::WrapInSharedEqual<std::tuple<Args...>, std::tuple<Deriveds...>>>(),
      slot,
      current_state,
      std::move(new_state),
      std::move(input)...
    );
}

template<typename Class, typename ReturnType, typename... Args>
template<typename... Deriveds>
StateBehavior<Class, ReturnType, Args...>&
StateBehavior<Class, ReturnType, Args...>::polymorphic()
{
  static_assert(
      drutility::detail::is_base_of_tuple_v<std::tuple<std::decay_t<Args>...>, std::tuple<std::decay_t<Deriveds>...>>,
      "Specified impossible polymorphic setting"
    );
  wrap_in_shared_equal_ = std::make_shared<detail::WrapInSharedEqual<
      std::tuple<Args...>,
      std::tuple<Deriveds...>
    >>();
  return *this;
}

template<typename Class, typename ReturnType, typename... Args>
template<typename T>
StateBehavior<Class, ReturnType, Args...>&
StateBehavior<Class, ReturnType, Args...>::returns(
    const std::string& state,
    std::enable_if_t<not std::is_same_v<ReturnType, void>, T>&& value
  )
{
  returns("", state, std::forward<T>(value));
  return *this;
}

template<typename Class, typename ReturnType, typename... Args>
template<typename T>
StateBehavior<Class, ReturnType, Args...>&
StateBehavior<Class, ReturnType, Args...>::returns(
    const std::string& slot,
    const std::string& state,
    std::enable_if_t<not std::is_same_v<ReturnType, void>, T>&& value
  )
{
  setResultSlot(slot);
  auto return_ptr = std::make_shared<std::decay_t<ReturnType>>(std::forward<T>(value));
  auto signal_ptr = nullptr;
  updateResultSlot(state, return_ptr, signal_ptr);
  return *this;
}

template<typename Class, typename ReturnType, typename... Args>
template<typename E>
StateBehavior<Class, ReturnType, Args...>&
StateBehavior<Class, ReturnType, Args...>::throws(
    const std::string& state,
    E&& excp
  )
{
  throws("", state, excp);
  return *this;
}

template<typename Class, typename ReturnType, typename... Args>
template<typename E>
StateBehavior<Class, ReturnType, Args...>&
StateBehavior<Class, ReturnType, Args...>::throws(
    const std::string& slot,
    const std::string& state,
    E&& excp
  )
{
  setResultSlot(slot);

  // Check if a result is already set for `state`.
  if (results_.find(state) != results_.end())
  {
      throw std::runtime_error{
          "Result already set for state '" + state + "'. Please check your mock object"
          " configuration."
        };
  }

  auto ptr = std::make_exception_ptr(excp);
  results_[state] = std::move(ptr);
  return *this;
}

template<typename Class, typename ReturnType, typename... Args>
template<typename... SigArgs>
StateBehavior<Class, ReturnType, Args...>&
StateBehavior<Class, ReturnType, Args...>::emits(
    const std::string& state,
    void (Class::*signal)(SigArgs...),
    SigArgs&&... args
  )
{
  emits("", state, signal, std::forward<SigArgs>(args)...);
  return *this;
}

template<typename Class, typename ReturnType, typename... Args>
template<typename... SigArgs>
StateBehavior<Class, ReturnType, Args...>&
StateBehavior<Class, ReturnType, Args...>::emits(
    const std::string& slot,
    const std::string& state,
    void (Class::*signal)(SigArgs...),
    SigArgs&&... args
  )
{
  setResultSlot(slot);
  auto return_ptr = nullptr;
  auto signal_ptr = std::make_shared<Signal<Class, SigArgs...>>(
      signal,
      std::forward<SigArgs>(args)...
    );
  updateResultSlot(state, return_ptr, signal_ptr);
  return *this;
}

template<typename Class, typename ReturnType, typename... Args>
std::variant<
    std::monostate,
    std::pair<
        std::shared_ptr<std::decay_t<ReturnType>>,
        std::shared_ptr<AbstractSignal<Class>>
      >,
    std::exception_ptr
  >
StateBehavior<Class, ReturnType, Args...>::call(const Args&... args)
{
  // Transition all slots.
  for (const auto& v : transitions_)
  {
    // Get current_state and transition table.
    std::string slot = v.first;
    std::string current_state = state_object_->get(slot);
    auto& map = v.second;  // state -> { (input..., target) }

    // ReturnTypes.
    bool found_match{false};
    std::string new_state;

    // Find a match. Search the wildcard (fallthrough) transitions
    // first, then look for a direct match.
    if (map.find("*") != map.end())
    {
      auto& vec = map.at("*");  // { (input..., target) }
      for (const auto& p : vec)
      {
        if ( match_pack_(p.first, args...) )
        {
          new_state = p.second;
          found_match = true;
          break;
        }
      }
    }
    if (map.find(current_state) != map.end())
    {
      auto& vec = map.at(current_state);  // { (input..., target) }
      for (const auto& p : vec)
      {
        if ( match_pack_(p.first, args...) )
        {
          new_state = p.second;
          found_match = true;
          break;
        }
      }
    }

    // Execute the transition.
    if (found_match)
    {
      state_object_->set(slot, new_state);
    }
  }

  // Get the slot's new state.
  auto state = state_object_->get(slot_);

  // Return the result if possible.
  if (results_.find(state) != results_.end())
  {
    return results_[state];
  }

  // If no direct result is found, check for a wildcard.
  if (results_.find("*") != results_.end())
  {
    return results_["*"];
  }

  // If no result was found, but the function is void, return void.
  if constexpr (std::is_same<std::decay_t<ReturnType>, void>::value)
  {
    return std::pair<
        std::shared_ptr<void>,
        std::shared_ptr<AbstractSignal<Class>>
      >{{}, {}};
  }

  // Otherwise, return nothing.
  return std::monostate{};
}

template<typename Class, typename ReturnType, typename... Args>
void
StateBehavior<Class, ReturnType, Args...>::setResultSlot(const std::string& slot)
{
  if (not fix_result_slot_)
  {
    fix_result_slot_ = true;
    slot_ = slot;
  }
  else if (slot_ != slot)
  {
    throw std::runtime_error{"Result slot already set to '" + slot_ + "'."};
  }
}

template<typename Class, typename ReturnType, typename... Args>
void
StateBehavior<Class, ReturnType, Args...>::updateResultSlot(
    const std::string& state,
    std::shared_ptr<std::decay_t<ReturnType>> return_ptr,
    std::shared_ptr<AbstractSignal<Class>> signal_ptr
  )
{
  auto it = results_.find(state);

  // If no result is registered for `state`, create one.
  if (it == results_.end())
  {
    results_[state] = Result{std::move(return_ptr), std::move(signal_ptr)};
    return;
  }

  // If there is a result, but it's not a return/emit, raise an error.
  if (not std::holds_alternative<Result>(it->second))
  {
    throw std::runtime_error{
        "Monostate/throw result already set for state '" + state + "'. Please check your mock"
        " object configuration."
      };
  }
  auto result = std::get<Result>(it->second);

  // If the result is return/emit with non-null return, throw instead of
  // overwriting the return. Otherwise, re-use the return value.
  if (result.first != nullptr)
  {
    if (return_ptr != nullptr)
    {
      throw std::runtime_error{
          "Return value already set for state '" + state + "'. Please check your mock object"
          " configuration."
        };
    }
    return_ptr = result.first;
  }

  // If the result is return/emit with non-null emit, throw instead of
  // overwriting the emit.
  if (result.second != nullptr)
  {
    if (signal_ptr != nullptr)
    {
      throw std::runtime_error{
          "Emit already set for state '" + state + "'. Please check your mock object"
          " configuration."
        };
    }
    signal_ptr = result.second;
  }

  // If none of this is true, set the return result, but don't delete
  // the emit result in the process!
  results_[state] = Result{return_ptr, signal_ptr};
}

template<typename Class, typename ReturnType, typename... Args>
StateBehavior<Class, ReturnType, Args...>&
StateBehavior<Class, ReturnType, Args...>::transition(
    const std::shared_ptr<detail::IWrapInSharedEqual<Args...>>& wrap_in_shared_equal,
    const std::string& slot,
    const std::string& current_state,
    std::string new_state,
    Args... input
  )
{
  // Throw if the new_state is the wildcard symbol `"*"`.
  if (new_state == "*")
  {
    throw std::runtime_error{"* not allowed as target state."};
  }

  // Register the slot in the `StateObject`.
  state_object_->get(slot);

  // Get the transitions for this slot.
  auto& map = transitions_[slot];  // state -> { (input..., target ) }
  auto& vec = map[current_state];  // { (input..., target) }

  // // Check for conflicts... A conflict arises if there are two
  // // transitions that match the same (slot, current_state, input...).
  // for (const auto& q : vec)
  // {
  //   if ( match_pack_(q.first, input...) )
  //   {
  //     throw std::runtime_error{"Transition conflict."};
  //   }
  // }

  // If all checks out, add the transition.
  vec.push_back(
      std::make_pair(
          wrap_in_shared_equal->wrap(std::move(input)...),
          std::move(new_state)
        )
    );
  return *this;
}

} // namespace drmock
