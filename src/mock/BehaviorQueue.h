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

#include "detail/IIsTuplePackEqual.h"
#include "AbstractBehavior.h"
#include "Behavior.h"

namespace drmock {

template<typename Result, typename... Args>
class BehaviorQueue final : public AbstractBehavior<Result, Args...>
{
  using DecayedResult = typename std::decay<Result>::type;

public:
  BehaviorQueue();
  BehaviorQueue(std::shared_ptr<detail::IIsTuplePackEqual<Args...>>);

  Behavior<Result, Args...>& push();
  Behavior<Result, Args...>& back();
  void enforce_order(bool);
  template<typename... Deriveds> void polymorphic();
  void setIsEqual(std::shared_ptr<detail::IIsTuplePackEqual<Args...>>) override;

  virtual std::variant<
      std::monostate,
      std::shared_ptr<DecayedResult>,
      std::exception_ptr
    > call(const Args&...) override;
  bool is_exhausted() const;

private:
  std::shared_ptr<detail::IIsTuplePackEqual<Args...>> is_tuple_pack_equal_{};
  std::vector<Behavior<Result, Args...>> behaviors_{};
  bool enforce_order_ = true;
};

} // namespace drmock

#include "BehaviorQueue.tpp"

#endif /* DRMOCK_SRC_MOCK_BEHAVIORSTACK_H */
