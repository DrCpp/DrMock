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

#ifndef DRMOCK_SRC_DRMOCK_MOCK_BEHAVIORSTACK_H
#define DRMOCK_SRC_DRMOCK_MOCK_BEHAVIORSTACK_H

#include <exception>
#include <memory>
#include <variant>
#include <vector>

#include <DrMock/mock/detail/IMakeTupleOfMatchers.h>
#include <DrMock/mock/AbstractBehavior.h>
#include <DrMock/mock/Behavior.h>

namespace drmock {

/**
 * A queue of `Behavior` objects that serves as standard implementation
 * of `AbstractBehavior`.
 *
 * See `AbstractBehavior` for details on template parameters.
 *
 * By default, if `BehaviorQueue::call` is called, then the input is
 * matched against the `Behavior` object `b` at the front of the queue.
 * If the input matches, then the return value of `b.produce()` is
 * returned. Otherwise, nothing is returned. The element at the front of
 * the queue remains in the queue as long as it persists. When this is
 * no longer the case, it is popped off the queue.
 *
 * `BehaviorQueue` has a member of type
 * `std::shared_ptr<detail::IMakeTupleOfMatchers<Args...>>`. This
 * _matching handler_ is shared with _all_ `Behavior` objects in the
 * queue. See `Behavior` for details.
 */
template<typename Class, typename ReturnType, typename... Args>
class BehaviorQueue final : public AbstractBehavior<Class, ReturnType, Args...>
{
  using Result = std::pair<
      std::shared_ptr<std::decay_t<ReturnType>>,
      std::shared_ptr<AbstractSignal<Class>>
    >;

public:
  BehaviorQueue();
  /**
   * @params make_tuple_of_matchers The tuple handler
   */
  BehaviorQueue(std::shared_ptr<detail::IMakeTupleOfMatchers<Args...>> make_tuple_of_matchers);

  /**
   * Push a new `Behavior` onto the queue and return a reference.
   */
  Behavior<Class, ReturnType, Args...>& push();

  /**
   * Access the last element pushed onto the queue.
   */
  Behavior<Class, ReturnType, Args...>& back();

  /**
   * Expect the behaviors on the queue to occur in order.
   */
  void enforce_order(bool);

  /**
   * Set the tuple handler of all future behaviors _and_ all behaviors
   * already enqueued to the default, with the specified polymorphic
   * type.
   *
   * @param Deriveds... The derived types for the default matcher
   *
   * See the general description of `Behavior` or
   * `Behavior::polymorphic` for details.
   */
  template<typename... Deriveds> void polymorphic();

  /**
   * See `AbstractBehavior::call`.
   *
   * Checks if the first element `b` of the queue matches `args...`. If
   * so, return the value of `b.produce()`. Otherwise, return
   * `std::monotstate`.
   * 
   * If `enforce_order` is set to `false`, then the entire queue is
   * instead searched from front to back for a matching element.
   */
  virtual std::variant<
      std::monostate,
      Result,
      std::exception_ptr
    > call(const Args&... args) override;

  /**
   * Check if all elements of the container are exhausted.
   */
  bool is_exhausted() const;

private:
  std::shared_ptr<detail::IMakeTupleOfMatchers<Args...>> make_tuple_of_matchers_{};  /**> The tuple handler object */
  // The queue is implemented as `std::vector`. Exhausted behaviors
  // remain in the vector. The "first" element of the queue is the first
  // persistent element.
  std::vector<Behavior<Class, ReturnType, Args...>> behaviors_{};  /**> The queue of behaviors */
  bool enforce_order_ = true;  /**> Expect the behaviors of the queue to occur in order */
};

} // namespace drmock

#include "BehaviorQueue.tpp"

#endif /* DRMOCK_SRC_DRMOCK_MOCK_BEHAVIORSTACK_H */
