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

#include "detail/IIsTuplePackEqual.h"
#include "AbstractBehavior.h"
#include "StateObject.h"

namespace drmock {

template<typename Result, typename... Args>
class StateBehavior final : public AbstractBehavior<Result, Args...>
{
public:
  StateBehavior();
  StateBehavior(
      std::shared_ptr<StateObject>
    );
  StateBehavior(
      std::shared_ptr<StateObject>,
      std::shared_ptr<detail::IIsTuplePackEqual<Args...>>
    );

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

  template<typename T = Result> StateBehavior& returns(
      const std::string& state,
      const std::enable_if_t<not std::is_same_v<Result, void>, T>& value
    );
  template<typename T = Result> StateBehavior& returns(
      const std::string& state,
      std::enable_if_t<not std::is_same_v<Result, void>, T>&& value
    );
  template<typename T = Result> StateBehavior& returns(
      const std::string& slot,
      const std::string& state,
      const std::enable_if_t<not std::is_same_v<Result, void>, T>&
    );
  template<typename T = Result> StateBehavior& returns(
      const std::string& slot,
      const std::string& state,
      std::enable_if_t<not std::is_same_v<Result, void>, T>&&
    );

  template<typename E> StateBehavior& throws(
      const std::string& state,
      E&& excp
    );
  template<typename E> StateBehavior& throws(
      const std::string& slot,
      const std::string& state,
      E&& excp
    );

  template<typename... Deriveds> StateBehavior& polymorphic();
  void setIsEqual(std::shared_ptr<detail::IIsTuplePackEqual<Args...>>) override;

  virtual std::variant<
      std::monostate,
      std::shared_ptr<typename std::decay<Result>::type>,
      std::exception_ptr
    > call(const Args&...) override;

private:
  using Vector = std::vector<std::pair<std::tuple<Args...>, std::string>>;

  void setResultSlot(const std::string& slot);
  void throwOnConflict(
      const std::string& slot,
      const std::string& state
    ) const;

  std::shared_ptr<StateObject> state_object_;
  std::shared_ptr<detail::IIsTuplePackEqual<Args...>> is_tuple_pack_equal_{};
  std::string slot_{};
  bool fixed_{false};
  std::map<
      std::string,
      std::variant<
          std::monostate,
          std::shared_ptr<typename std::decay<Result>::type>,
          std::exception_ptr
        >
    > results_{};
  std::map<std::string, std::map<std::string, Vector>> transitions_{}; 
  // slot -> { state -> { (input, target) } }
};

} // namespace drmock

#include "StateBehavior.tpp"

#endif /* DRMOCK_SRC_MOCK_STATEBEHAVIOR_H */
