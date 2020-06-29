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

#ifndef DRMOCK_SRC_MOCK_METHOD_H
#define DRMOCK_SRC_MOCK_METHOD_H

#include <list>
#include <memory>
#include <string>
#include <vector>

#ifdef DRMOCK_USE_QT
#include <QObject>
#include <QString>
#endif

#include "AbstractBehavior.h"
#include "Behavior.h"
#include "BehaviorQueue.h"
#include "IMethod.h"
#include "StateBehavior.h"
#include "StateObject.h"

namespace drmock {

/* Method

A mocked C++ method. Has a _name_ (with no current effect on behavior),
can be called and verified. The effect of the call is determined by an
AbstractBehavior, which must be configured prior to the call.

Per default, the AbstractBehavior is a BehaviorQueue. But using io() and
state(), the AbstractBehavior can be switched between the queue and a
StateBehavior.

When the Method is called with `args...`, the call is forwarded to the
currently selected behavior:

(1) Any produced std::exception_ptr is re-thrown.

(2) Any prodcued std::shared_ptr is returned.

(3) If the Method's return value is void, then a nullptr is returned.

If none of those occur, the call is considered to have _failed_. Any
future call of verify() will now return `false` (per default, it returns
`true`). An error message is printed, and then, If the return value of
Method is default constructible or void, then the default or a nullptr
is returned. Otherwise, `std::abort()` is called.

*** Implementation details: ***

* The currently used AbstractBehavior (`state_behavior_` or
  `behavior_queue_`) is `behavior_`, while the other is a nullptr.

* The sole purpose of `is_tuple_pack_equal_` is to be used as argument
  of AbstractBehavior::setIsEqual whenever the AbstractBehavior changes.
*/

template<typename Class, typename ReturnType, typename... Args>
class Method final : public IMethod
{
  using DecayedReturnType = typename std::decay<ReturnType>::type;

public:
  Method();
  Method(std::string);
  Method(std::shared_ptr<StateObject>);
  Method(std::string, std::shared_ptr<StateObject>);

  // Set the comparison method of `behavior_queue_` or `state_behavior_`
  // (depending on which is active) to `IsEqual<tuple<Args...>, tuple<Deriveds...>>`.
  template<typename... Deriveds> void polymorphic();

  // Enable BehaviorQueue usage and return a reference to the queue.
  BehaviorQueue<Class, ReturnType, Args...>& io();
  Behavior<Class, ReturnType, Args...>& push();
  void enforce_order(bool);

  // Enable StateBehavior usage and return a reference to the state
  // behavior.
  StateBehavior<Class, ReturnType, Args...>& state();

  // Per default, return `true`. If an error occured (see above), return
  // `false`.
  bool verify() const override;
  const std::vector<std::vector<std::string>>& error_msgs() const;
  std::string makeFormattedErrorString() const override;

  std::shared_ptr<DecayedReturnType> call(const Args&...);

  void parent(Class* parent);

private:
  std::string name_{};
  std::shared_ptr<detail::IIsTuplePackEqual<Args...>> is_tuple_pack_equal_{};
  std::shared_ptr<StateObject> state_object_{};
  std::shared_ptr<StateBehavior<Class, ReturnType, Args...>> state_behavior_{};
  std::shared_ptr<BehaviorQueue<Class, ReturnType, Args...>> behavior_queue_{};
  std::shared_ptr<AbstractBehavior<Class, ReturnType, Args...>> behavior_{};
  bool has_failed_ = false;
  std::vector<std::vector<std::string>> error_msgs_{};

  Class* parent_;
};

} // namespace drmock

#include "Method.tpp"

#endif /* DRMOCK_SRC_MOCK_METHOD_H */
