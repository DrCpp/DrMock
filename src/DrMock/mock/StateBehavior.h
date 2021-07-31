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


/**
 * State machine implementation of `AbstractBehavior`.
 *
 * See `AbstractBehavior` for details on template parameters.
 *
 * Every state behavior has a set of _slots_, each of which has a
 * _current state_. The states change ("transition") according to a
 * _transition table_ whenever `StateBehavior::call` is called. The
 * return value of `StateBehavior::call` depends on the current state of
 * the _result slot_, which is set by the user, as well, **after**
 * transition.
 *
 * Per default, every state behavior has the slot `""`, also known as
 * _default slot_. The slots and states are managed by an internal
 * `StateObject`, which may be shared with other `StateBehavior`
 * objects.
 *
 * Every `StateBehavior` has a member of type
 * `std::shared_ptr<detail::IMakeTupleOfMatchers<Args...>>`. The job of
 * this _matching handler_ is to wrap expected values passed to the
 * behavior via `transition` in `std::shared_ptr<IMatcher<Args>>...`
 * objects, which are used to check if an input matches some entry of
 * the transition table. By default, they are wrapped in
 * `std::shared_ptr<Equal<Args>>...` objects.
 *
 * If `polymorphic<Deriveds...>()` is called, a new handler is created
 * which wraps expected values in
 * `std::shared_ptr<Equal<Args, Deriveds>>...` objects. This means that
 * when polymorphic objects are used in `call`, then they are matched
 * against the expected behaviors by comparing them as `Deriveds...`,
 * not as base types.
 *
 * The entries of the _transition table_ take the following form:
 * 
 * ```
 * (slot, current_state, matcher, new_state)
 * ```
 * 
 * "On `call(input...)`, if `matcher->match(input)` and `old_state`
 * matches the state of `slot`, then transition `slot` to `new_state`."
 * If this occurs, we say that the `(old_state, input...)` matches the
 * entry of the transition table.
 * 
 * Two states _match_ if they are equal as strings or if one of them is
 * `"*"` (the catch-all state).
 * 
 * Beware of inconsistencies in the transition table. It is possible that
 * `(current_state, input...)` matches multiple entries of the transition
 * table with the same slot (this depends on the `matcher`). If this is
 * the case, the transition that is executed is undefined.
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
  /**
   * @param state_object The internal state object
   */
  StateBehavior(std::shared_ptr<StateObject> state_object);
  /**
   * @param state_object The internal state object
   * @param make_tuple_of_matchers The matching handler
   */
  StateBehavior(
      std::shared_ptr<StateObject> state_object,
      std::shared_ptr<detail::IMakeTupleOfMatchers<Args...>> make_tuple_of_matchers
    );

  /**
   * Add a transition for the default slot.
   *
   * @param current_state The state to transition from
   * @param new_state The state to transition to
   * @param input... The expected input or matchers of the transition
   *
   * Is equivalent to
   * `transition("", current_state, new_state, * input...);`. See the
   * documentation of 
   * `StateBehavior::transition(const std::string&, const std::string&, std::string, detail::expect_t<Args>...)`
   * for details.
   */
  StateBehavior& transition(
      const std::string& current_state,
      std::string new_state,
      detail::expect_t<Args>... input
    );
  /**
   * Add a transition.
   *
   * @param slot The slot for which the transition holds
   * @param current_state The state to transition from
   * @param new_state The state to transition to
   * @param input... The expected input or matchers of the transition
   *
   * For every `i=0, ..., sizeof...(Args)`, the ith component of
   * `args...` may be of type `Args[i]` (a _raw expected value_) or
   * `std::shared_ptr<IMatcher<Args[i]>>` (a matcher). Raw expected
   * values are wrapped in an
   * `std::shared_ptr<Equal<Args[i], Deriveds[i]>>` by the matching
   * handler. This allows the consumer to use custom matchers alongside
   * the standard equality matcher `Equal`.
   *
   * The new entry specifies that, if `slot` has current state `state`
   * and `StateBehavior::call` is called with `input...` as arguments,
   * then `slot` transitions to `new_state`.
   */
  StateBehavior& transition(
      const std::string& slot,
      const std::string& current_state,
      std::string new_state,
      detail::expect_t<Args>... input
    );

  /**
   * Convenience function which combines
   * `StateBehavior::transition(const std::string&, std::string, detail::expect_t<Args>...)`
   * with a one-shot `polymorphic<Deriveds...>()` call.
   *
   * See `void polymorphic()` and
   * `StateBehavior::transition(const std::string&, std::string, detail::expect_t<Args>...)`
   * for details.
   */
  template<typename... Deriveds>
  StateBehavior& transition(
      const std::string& current_state,
      std::string new_state,
      detail::expect_t<Args>... input
    );

  /**
   * Convenience function which combines
   * `StateBehavior::transition(const std::string&, const std::string&, std::string, detail::expect_t<Args>...)`
   * with a one-shot `polymorphic<Deriveds...>()` call.
   *
   * See `void polymorphic()` and
   * `StateBehavior::transition(const std::string&, const std::string&, std::string, detail::expect_t<Args>...)`
   * for details.
   */
  template<typename... Deriveds>
  StateBehavior& transition(
      const std::string& slot,
      const std::string& current_state,
      std::string new_state,
      detail::expect_t<Args>... input
    );

  /**
   * Set a return value for a state on the default slot.
   *
   * @param state The state to return on
   * @param value The value to return
   *
   * On the first call (of any overload of `StateBehavior::returns`),
   * the result slot is set to `""`.
   *
   * Equivalent to `StateBehavior::returns("", state, value)`.
   */
  template<typename T = ReturnType> StateBehavior& returns(
      const std::string& state,
      std::enable_if_t<not std::is_same_v<ReturnType, void>, T>&& value
    );
  /**
   * Set a return value for a slot/state.
   *
   * @param slot The result slot
   * @param state The state to return on
   * @param value The value to return
   *
   * On the first call (of any overload of `StateBehavior::returns`),
   * the result slot is set to `slot`.
   */
  template<typename T = ReturnType> StateBehavior& returns(
      const std::string& slot,
      const std::string& state,
      std::enable_if_t<not std::is_same_v<ReturnType, void>, T>&& value
    );

  /**
   * Add a throw for the default slot.
   *
   * @param state The state to return on
   * @param excp The exception to throw
   *
   * On the first call (of any overload of `StateBehavior::returns`),
   * the result slot is set to `""`.
   *
   * Equivalent to `StateBehavior::returns("", state, excp)`.
   */
  template<typename E> StateBehavior& throws(
      const std::string& state,
      E&& excp
    );
  /**
   * Add a throw for a slot/state.
   *
   * @param slot The result slot
   * @param state The state to throw on
   * @param excp The exception to throw
   *
   * On the first call (of any overload of `StateBehavior::returns`),
   * the result slot is set to `slot`.
   */
  template<typename E> StateBehavior& throws(
      const std::string& slot,
      const std::string& state,
      E&& excp
    );

  /**
   * Add a Qt signal emit for the default slot.
   *
   * @param state The state to emit on
   * @param signal The signal to emit
   * @param args... The arguments passed to the signal
   *
   * On the first call (of any overload of `StateBehavior::returns`),
   * the result slot is set to `""`.
   *
   * Equivalent to `StateBehavior::emits("", state, signal, args...)`.
   */
  template<typename... SigArgs> StateBehavior& emits(
      const std::string& state,
      void (Class::*signal)(SigArgs...),
      SigArgs&&...
    );
  /**
   * Add a Qt signal emit for the default slot.
   *
   * @param slot The result slot
   * @param state The state to emit on
   * @param signal The signal to emit
   * @param args... The arguments passed to the signal
   *
   * On the first call (of any overload of `StateBehavior::returns`),
   * the result slot is set to `slot`.
   */
  template<typename... SigArgs> StateBehavior& emits(
      const std::string& state,
      const std::string& slot,
      void (Class::*)(SigArgs...),
      SigArgs&&...
    );

  /**
   * Define what polymorhism is used when wrapping naked `Args...` into
   * a matcher by replacing the matching handler with a new
   * `std::shared_ptr<detail::MakeTupleOfMatchers<std::tuple<Args...>, std::tuple<Deriveds...>>>`.
   *
   * This call will not change previously made entries of the transition
   * table, only futures entries.
   */
  template<typename... Deriveds> StateBehavior& polymorphic();

  /**
   * See `AbstractBehavior::call`.
   *
   * - For every slot, find a matching entry
   *   ```
   *   (slot, current_state, matcher, new_state)
   *   ```
   *   of the transition table. If found, set the slot's state to
   *   `new_state`. If no matching transition is found, search the
   *   wildcard transitions for a matching entry:
   *   ```
   *   (slot, "*", matcher, new_state)
   *   ```
   *   If found, set the slot's state to `new_state`.
   *
   * - Take the (new) state of the result slot and look it up in the
   *   result table:
   *   + If the result type is void, return `{}`.
   *   + Otherwise, if a result (return value or exception pointer) is
   *     found, return it. If no result is found, check if a return
   *     statement for the wildcard state `"*"` exists and, if so,
   *     return the result.
   *   + Otherwise, return `std::monotstate{}`
   */
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
      const std::shared_ptr<detail::IMakeTupleOfMatchers<Args...>>& make_tuple_of_matchers,
      const std::string& slot,
      const std::string& current_state,
      std::string new_state,
      detail::expect_t<Args>... input
    );

  std::shared_ptr<StateObject> state_object_;
  std::shared_ptr<detail::IMakeTupleOfMatchers<Args...>> make_tuple_of_matchers_{};
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
