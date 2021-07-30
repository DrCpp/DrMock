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

#ifndef DRMOCK_SRC_DRMOCK_MOCK_METHOD_H
#define DRMOCK_SRC_DRMOCK_MOCK_METHOD_H

#include <memory>
#include <string>
#include <vector>

#include <DrMock/mock/AbstractBehavior.h>
#include <DrMock/mock/Behavior.h>
#include <DrMock/mock/BehaviorQueue.h>
#include <DrMock/mock/IMethod.h>
#include <DrMock/mock/StateBehavior.h>
#include <DrMock/mock/StateObject.h>

namespace drmock {

/**
 * Simulates the behavior of a C++ method.
 *
 * @tparam Class The class that owns the simulated method
 * @tparam ReturnType The return type of the simulated method
 * @tparam Args... The parameter types of the simulated method
 *
 * The `Method` object can be called and verified (check if any
 * unexpected calls have occured in the past). The effect of calling a
 * `Method` is controlled by an instance of `AbstractBehavior`. By
 * default, this `AbstractBehavior` is a `BehaviorQueue`.
 *
 * Changing the behavior type from `BehaviorQueue` to `StateBehavior` or
 * back will not overwrite any previously recorded behavior.
 * Nevertheless, there is no reason to change the behavior type more
 * than once.
 *
 * The `Method` class shares an object of type
 * `std::shared_ptr<detail::IMakeTupleOfMatchers<Args...>>` with its
 * behaviors. The job of this _matching handler_ is to wrap expected
 * values passed to the behaviors during configuration in
 * `std::shared_ptr<IMatcher<Args>>...` objects. By default, they are
 * wrapped in `std::shared_ptr<Equal<Args>>...` objects.
 *
 * If `polymorphic<Deriveds...>()` is called, a new handler is created
 * which wraps expected values in
 * `std::shared_ptr<Equal<Args, Deriveds>>...` objects. This means that
 * when polymorphic objects are used in `call`, then they are matched
 * against the expected behaviors by comparing them as `Deriveds...`,
 * not as base types.
 *
 * See `Behavior`, `BehaviorQueue` or `StateBehavior` for details.
 */
template<typename Class, typename ReturnType, typename... Args>
class Method final : public IMethod
{
  using DecayedReturnType = typename std::decay<ReturnType>::type;

public:
  Method();
  /**
   * @param name The name of the method
   */
  Method(std::string name);
  /**
   * @param state_object The state_object shared with internal state behavior
   *
   * The `state_object` parameter is for dependency injection only and
   * should not be used by a consumer.
   */
  Method(std::shared_ptr<StateObject> state_object);
  /**
   * @param name The name of the method
   * @param state_object The state_object shared with internal state behavior
   *
   * The `state_object` parameter is for dependency injection only and
   * should not be used by a consumer.
   */
  Method(std::string name, std::shared_ptr<StateObject> state_object);

  /**
   * Replace the matching handler of all behaviors with a new
   * `std::shared_ptr<detail::MakeTupleOfMatchers<std::tuple<Args...>, std::tuple<Deriveds...>>>`.
   *
   * Note that this means that the previous `polymorphic` configurations
   * of the state and queue behaviors are overwritten.
   *
   * See the general section of `Method` for what this means in detail.
   */
  template<typename... Deriveds> void polymorphic();

  /**
   * Enable `BehaviorQueue` usage and return a reference.
   */
  BehaviorQueue<Class, ReturnType, Args...>& io();

  /**
   * Convenience method; calls `push()` on the internal behavior queue
   * (even if `BehaviorQueue` is not active).
   */
  Behavior<Class, ReturnType, Args...>& push();

  /**
   * Convenience method; calls `enforce_order()` on the internal
   * behavior queue (even if `BehaviorQueue` is not active).
   */
  void enforce_order(bool);

  /**
   * Enable `StateBehavior` usage and return a reference.
   */
  StateBehavior<Class, ReturnType, Args...>& state();

  /**
   * Check if any calls have failed.
   */
  bool verify() const override;

  /**
   * Return error messages for failed calls.
   *
   * @returns A row-major matrix, rows are failed calls, columns contain
   *   the arguments of these calls.
   */
  const std::vector<std::vector<std::string>>& error_msgs() const;

  /**
   * Return a printable version of the error messages.
   */
  std::string makeFormattedErrorString() const override;

  /**
   * When the Method is called with `args...`, the call is forwarded to the
   * currently selected behavior:
   * 
   * - Any produced std::exception_ptr is re-thrown.
   * - Any produced std::shared_ptr is returned.
   * - If the Method's return value is void, then a nullptr is returned.
   * 
   * If none of those occur, the call is considered to have _failed_. Any
   * future call of verify() will now return `false` (per default, it returns
   * `true`). An error message is printed, and then, If the return value of
   * Method is default constructible or void, then the default or a nullptr
   * is returned. Otherwise, `std::abort()` is called.
   */
  std::shared_ptr<DecayedReturnType> call(const Args&...);

  /**
   * Set the object that owns the method.
   */
  void parent(Class*);

private:
  std::string name_{};
  std::shared_ptr<detail::IMakeTupleOfMatchers<Args...>> make_tuple_of_matchers_{};
  std::shared_ptr<StateObject> state_object_{};  /**> Internal state object. Only used for dependency injection. */
  // The currently used AbstractBehavior (`state_behavior_` or
  // `behavior_queue_`) is `behavior_`, while the other is a nullptr.
  std::shared_ptr<StateBehavior<Class, ReturnType, Args...>> state_behavior_{};
  std::shared_ptr<BehaviorQueue<Class, ReturnType, Args...>> behavior_queue_{};
  std::shared_ptr<AbstractBehavior<Class, ReturnType, Args...>> behavior_{};
  bool has_failed_ = false;  /**> `true` if `call` encountered unexpected args. */
  std::vector<std::vector<std::string>> error_msgs_{};
  Class* parent_;  /**> Pointer to the owner of the method. */
};

} // namespace drmock

#include "Method.tpp"

#endif /* DRMOCK_SRC_DRMOCK_MOCK_METHOD_H */
