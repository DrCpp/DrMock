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

#ifndef DRMOCK_SRC_MOCK_STATEBEHAVIOR_H
#define DRMOCK_SRC_MOCK_STATEBEHAVIOR_H

#include <exception>
#include <map>
#include <memory>
#include <variant>

#include "detail/IWrapInSharedEqual.h"
#include "AbstractBehavior.h"
#include "detail/InvokeOnPack.h"
#include "StateObject.h"

namespace drmock {

/* StateBehavior

(See also: AbstractBehavior.) Holds a StateObject, whose states are
changed by calling by and determine the return value of call().

* The entries of the _transition table_ take the following form:

    (slot, oldState, tuple(input...), newState)

  Read: "On `input...`, transition `slot` from `oldState` (if that is its
  state) to `newState`."

  A special state is the wildcard state `"*"`, which acts as a fallthru
  state (see below).

* The transition table may never have any inconsitencies. Therefore, if
  a transition is pushed that violates the following rules, an exception
  is thrown:

  (1) There may be no two entries with

        slot1     is equal to slot2,
        oldState1 is equal to oldState2, and
        input1... is equal to input2... (using `wrap_in_shared_equal_`),

      except if `oldState` is the wildcard state `"*"`.

  (2) The wildcard state `"*"` may never occur as `newState`.

* The return/result value of call() is determined by a _result slot_.
  The _return/result table_ is simply

    map: state (of result slot) -> (return value, emit)

* The result table may never have any inconsitencies. Therefore, if
  a transition is pushed that violates the following rules, an exception
  is thrown:

  (1) There may be not two entries with

        state1 is equal to state2.

      An entry with state equal to `"*"` is allowed, even alongside
      other entries (see below).

* The effect of `call(input...)` is as follows:

  (1) For every slot, find a matching entry

        (slot, oldState, tuple(input...), newState)

      of the transition table (i.e. `slot` and `input...` match and
      `oldState` is equal to `state_object_->get(slot)`). If found,
      set the slot's state to `newState`.

      If no matching transition is found, search the wildcard
      transitions for a matching entry:

        (slot, "*", tuple(input...), newState)

      (i.e. `slot` and `input...` match). If found, set the slot's state
      to `newState`.

  (2) Take the (new) state of the result slot and look it up in the
      result table. If a result (return value of exception pointer) is
      found, return that. If no result is found, check if a return
      statement for the wildcard state `"*"` exists. If yes, then return
      its result. (In other words, concrete entries that precedence over
      the wildcard state, which acts as a "catch-all".) Otherwise, if
      the result type is void, return {}. Else, return an
      `std::monostate`.

*** Implementation details: ***

* The transition table is implemented as follows:

    map: slot -> { map: oldState -> { vector : (input..., newState) } }

  A vector is used instead of a map/unordered_map to prevent the need for
  an operator</std::hash.

* The sole purpose of `wrap_in_shared_equal_` is to be used as argument
  of AbstractBehavior::setIsEqual whenever the AbstractBehavior changes.
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
  StateBehavior(
      std::shared_ptr<StateObject>
    );
  StateBehavior(
      std::shared_ptr<StateObject>,
      std::shared_ptr<detail::IWrapInSharedEqual<Args...>>
    );

  // Add transition for default slot (resp. `slot`). Throws according to
  // the rules above.
  StateBehavior& transition(
      std::string current_state,
      std::string new_state,
      Args... input
    );
  StateBehavior& transition(
      const std::string& slot,
      const std::string& current_state,
      std::string new_state,
      Args... input
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
  void setIsEqual(std::shared_ptr<detail::IWrapInSharedEqual<Args...>>) override;

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

  std::shared_ptr<StateObject> state_object_;
  std::shared_ptr<detail::IWrapInSharedEqual<Args...>> wrap_in_shared_equal_{};
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
          std::vector<std::pair<std::tuple<std::shared_ptr<ICompare<Args>>...>, std::string>>
        >
    > transitions_{};  // slot -> { state -> { (input, target) } }
  detail::InvokeOnPack<std::tuple<Args...>> invoke_on_pack_{};
};

} // namespace drmock

#include "StateBehavior.tpp"

#endif /* DRMOCK_SRC_MOCK_STATEBEHAVIOR_H */
