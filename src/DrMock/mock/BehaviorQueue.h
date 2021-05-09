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

#ifndef DRMOCK_SRC_MOCK_BEHAVIORSTACK_H
#define DRMOCK_SRC_MOCK_BEHAVIORSTACK_H

#include <exception>
#include <memory>
#include <variant>
#include <vector>

#include <DrMock/mock/detail/IIsTuplePackEqual.h>
#include <DrMock/mock/AbstractBehavior.h>
#include <DrMock/mock/Behavior.h>

namespace drmock {

/* BehaviorQueue

(See also: AbstractBehavior.) A container of Behavior objects that acts
as AbstractBehavior.

When `call(args...)` is called, the BehaviorQueue checks if the _front_
element of the queue (see implementation details below) matches
`args...`. If yes, the front behavior produces and the output is
forwarded. Otherwise, std::monotstate (no output) is returned.

If `enforce_order` is set to `false`, then the entire queue is instead
searched from front to back for a matching element.

*** Implementation details: ***

* The queue is implemented as std::vector, and, once pushed/enqueued, a
  Behavior instance never leaves the queue. Instead, the _front_ of the
  queue is determined by the first element that still persists.

* The sole purpose of `is_tuple_pack_equal_` is to be used as argument
  of Behavior::setIsEqual whenever new elements are pushed onto the
  queue.
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
  BehaviorQueue(std::shared_ptr<detail::IIsTuplePackEqual<Args...>>);

  Behavior<Class, ReturnType, Args...>& push();
  Behavior<Class, ReturnType, Args...>& back();
  void enforce_order(bool);

  // Set `is_tuple_pack_equal_` and call `setIsEqual` for *all* elements
  // of the queue.
  template<typename... Deriveds> void polymorphic();
  void setIsEqual(std::shared_ptr<detail::IIsTuplePackEqual<Args...>>) override;

  virtual std::variant<
      std::monostate,
      Result,
      std::exception_ptr
    > call(const Args&...) override;

  // Check if all elements of the container are exhausted.
  bool is_exhausted() const;

private:
  std::shared_ptr<detail::IIsTuplePackEqual<Args...>> is_tuple_pack_equal_{};
  std::vector<Behavior<Class, ReturnType, Args...>> behaviors_{};
  bool enforce_order_ = true;
};

} // namespace drmock

#include "BehaviorQueue.tpp"

#endif /* DRMOCK_SRC_MOCK_BEHAVIORSTACK_H */
