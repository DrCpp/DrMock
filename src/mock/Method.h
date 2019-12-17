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
#include <vector>
#include <memory>

#include "IMethod.h"
#include "Behavior.h"
#include "BehaviorStack.h"
#include "StateObject.h"
#include "StateBehavior.h"

namespace drmock {

template<typename Result, typename... Args>
class Method final : public IMethod
{
  using DecayedResult = typename std::decay<Result>::type;

public:
  Method();
  Method(std::string);
  Method(std::shared_ptr<StateObject>);
  Method(std::string, std::shared_ptr<StateObject>);
  template<typename... Deriveds> void polymorphic();

  BehaviorStack<Result, Args...>& io();
  Behavior<Result, Args...>& push();
  void enforce_order(bool);

  StateBehavior<Result, Args...>& state();

  bool verify() const override;
  const std::vector<std::vector<std::string>>& error_msgs() const;
  std::string makeFormattedErrorString() const;

  std::shared_ptr<DecayedResult> call(const Args&...);

private:
  std::string name_{};
  std::shared_ptr<detail::IIsTuplePackEqual<Args...>> is_tuple_pack_equal_{};
  std::shared_ptr<StateObject> state_object_{};
  std::shared_ptr<StateBehavior<Result, Args...>> state_behavior_{};
  std::shared_ptr<BehaviorStack<Result, Args...>> behavior_stack_{};
  std::shared_ptr<AbstractBehavior<Result, Args...>> behavior_{};
  bool has_failed_ = false;
  std::vector<std::vector<std::string>> error_msgs_{};
};

} // namespace drmock

#include "Method.tpp"

#endif /* DRMOCK_SRC_MOCK_METHOD_H */
