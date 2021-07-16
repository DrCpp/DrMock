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

#ifndef DRMOCK_SRC_DRMOCK_MOCK_STATEBEHAVIOR_H
#define DRMOCK_SRC_DRMOCK_MOCK_STATEBEHAVIOR_H

#include <exception>
#include <map>
#include <memory>
#include <variant>

#include <DrMock/mock/detail/IMakeTupleOfMatchers.h>
#include <DrMock/mock/AbstractBehavior.h>
#include <DrMock/mock/detail/MatchPack.h>
#include <DrMock/mock/StateObject.h>

namespace drmock {

/* StateBehavior

(See also: `AbstractBehavior`.) Holds a StateObject, whose states are
changed by `AbstractBehavior::call()` and determine the return value of
`AbstractBehavior::call()`.

* The entries of the _transition table_ take the following form:

    (slot, old_state, matcher, new_state)

  Read: "On `call(input...)`, if `matcher->match(input)` and `old_state`
  matches the state of `slot`, then transition `slot` to `new_state`."
  If this occurs, we say that the `(old_state, input...)` matches the
  entry of the transition table.

  Two states _match_ if they are equal as strings or if one of them is
  `"*"` (the catch-all state, see below).

  Beware of inconsistencies in the transition table. It is possible that
  `(old_state, input...)` matches multiple entries of the transition
  table with the same slot (this depends on the `matcher`). If this is
  the case, the transition that is executed is undefined.

* The return/result value of call() is determined by a _result slot_.
  The _return/result table_ is simply

    map: state (of result slot) -> (return value, emit)

* The result table may never have any inconsitencies. Therefore, if
  a transition is pushed that violates the following rule, an exception
  is thrown: 

  (1) There may be not two entries with

        `state1` is equal to `state2`.

      An entry with state equal to `"*"` is allowed, even alongside
      other entries (see below).

* The effect of `call(input...)` is as follows:

  (1) For every slot, find a matching entry

        (slot, old_state, matcher, new_state)

      of the transition table. If found, set the slot's state to
      `new_state`.

      If no matching transition is found, search the wildcard
      transitions for a matching entry:

        (slot, "*", matcher, new_state)

      If found, set the slot's state to `new_state`.

  (2) Take the (new) state of the result slot and look it up in the
      result table:

      - If the result type is void, return {}.
      - Otherwise, if a result (return value or exception pointer) is
        found, return it. If no result is found, check if a return
        statement for the wildcard state `"*"` exists and, if so, return
        the result.
      - Otherwise, return `std::monotstate{}`.

The above does not take into account Qt signals. Please refer to the
implementation for details.
*/

template<typename Class, typename ReturnType, typename... Args>
class StateBehavior final : public AbstractBehavior<Class, ReturnType, Args...>
{
public:
  using Result = std::pair<
                     std::shared_ptr<std::decay_t<ReturnType>>,
                     std::shared_ptr<AbstractSignal<Class>>
                   >;

  StateBehavior();
  StateBehavior(std::shared_ptr<StateObject>);
  StateBehavior(
      std::shared_ptr<StateObject>,
      std::shared_ptr<detail::IMakeTupleOfMatchers<Args...>>
    );

  // Add transition for default slot (resp. `slot`). Throws according to
  // the rules above.
  StateBehavior& transition(
      const std::string& current_state,
      std::string new_state,
      detail::expect_t<Args>... input
    );
  StateBehavior& transition(
      const std::string& slot,
      const std::string& current_state,
      std::string new_state,
      detail::expect_t<Args>... input
    );

  // Convenience function for one-time polymorphic specification.
  template<typename... Deriveds>
  StateBehavior& transition(
      const std::string& current_state,
      std::string new_state,
      detail::expect_t<Args>... input
    );
  template<typename... Deriveds>
  StateBehavior& transition(
      const std::string& slot,
      const std::string& current_state,
      std::string new_state,
      detail::expect_t<Args>... input
    );

  // Add entry to result table - see above for details.
  //
  // On first call, set the result slot to default slot (resp. `slot`).
  // Not available for void methods.
  template<typename T = ReturnType> StateBehavior& returns(
      const std::string& state,
      std::enable_if_t<not std::is_same_v<ReturnType, void>, T>&& value
    );
  template<typename T = ReturnType> StateBehavior& returns(
      const std::string& slot,
      const std::string& state,
      std::enable_if_t<not std::is_same_v<ReturnType, void>, T>&&
    );

  // Add throw for default slot (resp. `slot`).
  template<typename E> StateBehavior& throws(
      const std::string& state,
      E&& excp
    );
  template<typename E> StateBehavior& throws(
      const std::string& slot,
      const std::string& state,
      E&& excp
    );

  // Add emit command to result table - see above for details.
  template<typename... SigArgs> StateBehavior& emits(
      const std::string& state,
      void (Class::*)(SigArgs...),
      SigArgs&&...
    );
  template<typename... SigArgs> StateBehavior& emits(
      const std::string& state,
      const std::string& slot,
      void (Class::*)(SigArgs...),
      SigArgs&&...
    );

  // Setter for `wrap_in_shared_equal_`.
  template<typename... Deriveds> StateBehavior& polymorphic();

  virtual std::variant<
      std::monostate,
      Result,
      std::exception_ptr
    > call(const Args&...) override;

private:
  // Set the result slot if not already set. Throw if the result slot is
  // set and `slot` is not equal to `slot_` or if `state` already occurs
  // in an entry of the result table.
  void setResultSlot(const std::string& slot);
  bool fix_result_slot_{false};  // Remember if result slot is set.

  void updateResultSlot(
      const std::string& state,
      std::shared_ptr<std::decay_t<ReturnType>> rtn,
      std::shared_ptr<AbstractSignal<Class>> signal
    );

  // Implementation detail of public `transition` methods.
  StateBehavior& transition(
      const std::shared_ptr<detail::IMakeTupleOfMatchers<Args...>>& wrap_in_shared_equal,
      const std::string& slot,
      const std::string& current_state,
      std::string new_state,
      detail::expect_t<Args>... input
    );

  std::shared_ptr<StateObject> state_object_;
  std::shared_ptr<detail::IMakeTupleOfMatchers<Args...>> wrap_in_shared_equal_{};
  std::string slot_{};
  std::map<
      std::string,
      std::variant<
          std::monostate,
          Result,
          std::exception_ptr
        >
    > results_{};
  std::map<
      std::string,
      std::map<
          std::string,
          std::vector<std::pair<std::tuple<std::shared_ptr<IMatcher<Args>>...>, std::string>>
        >
    > transitions_{};  // slot -> { state -> { (input, target) } }
  detail::MatchPack<std::tuple<Args...>> match_pack_{};
};

} // namespace drmock

#include "StateBehavior.tpp"

#endif /* DRMOCK_SRC_DRMOCK_MOCK_STATEBEHAVIOR_H */
