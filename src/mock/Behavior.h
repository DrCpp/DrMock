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

#ifndef DRMOCK_SRC_MOCK_BEHAVIOR_H
#define DRMOCK_SRC_MOCK_BEHAVIOR_H

#include <memory>
#include <optional>
#include <variant>

#include "AbstractBehavior.h"
#include "detail/IsTuplePackEqual.h"

namespace drmock {

template<typename, typename...> class Method;

template<typename Result, typename... Args>
class Behavior
{
public:
  Behavior();
  Behavior(std::shared_ptr<detail::IIsTuplePackEqual<Args...>>);

  template<typename T = std::tuple<Args...>>
    typename std::enable_if<(std::tuple_size<T>::value > 0), Behavior&>::type expects();
  Behavior& expects(Args...);
  template<typename T> Behavior& returns(T&&);
  template<typename E> Behavior& throws(E&&);
  Behavior& times(unsigned int);
  Behavior& persists();
  template<typename... Deriveds> Behavior& polymorphic();

  bool is_persistent() const;
  bool match(const Args&...) const;
  std::variant<
      std::shared_ptr<typename std::decay<Result>::type>,
      std::exception_ptr
    > produce();
  void setIsEqual(std::shared_ptr<detail::IIsTuplePackEqual<Args...>>);

private:
  std::optional<std::tuple<Args...>> expect_{};
  std::shared_ptr<typename std::decay<Result>::type> result_{};
  std::exception_ptr exception_{};
  unsigned int times_ = 1;
  bool persists_ = false;
  std::shared_ptr<detail::IIsTuplePackEqual<Args...>> is_tuple_pack_equal_{};
};

} // namespace

#include "Behavior.tpp"

#endif /* DRMOCK_SRC_MOCK_BEHAVIOR_H */
