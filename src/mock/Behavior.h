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

#include "detail/InvokeOnPack.h"
#include "detail/IWrapInSharedEqual.h"
#include "AbstractSignal.h"

namespace drmock {

/* Behavior

Class template that represents a method's behavior.

+ A Behavior has a parent (Class, the object whose behavior is
  specified) return type (ReturnType) and parameter types (Args...).

+ A Behavior can _expect_ a certain _input_ (a set of arguments that
  match the parameter types).

+ A Behavior can _produce_ either a pair (first: shared pointer to an
  instance of its return type, second: shared_ptr to an `AbstractSignal`
  object), or an std::exception_ptr.

+ A Behavior has a life span. After a set number of productions
  (which may be infinite), the Behavior object no longer _persists_.

+ The results of the production and the life span must be configured by
  the user.

Making conflicting configuration calls, such as `returns` followed by
`throws` will raise an error.
*/

template<typename Class, typename ReturnType, typename... Args>
class Behavior
{
public:
  using Result = std::pair<
                     std::shared_ptr<std::decay_t<ReturnType>>,
                     std::shared_ptr<AbstractSignal<Class>>
                   >;

  Behavior();
  Behavior(std::shared_ptr<detail::IWrapInSharedEqual<Args...>>);

  // Expect any argument.
  template<typename T = std::tuple<Args...>>
  std::enable_if_t<(std::tuple_size_v<T> > 0), Behavior&> expects();

  // Set the expected arguments, return value, emit or thrown exception.
  // Note: Non-template overload is always prefered according to the C++
  // spec.

  Behavior& expects(detail::expect_t<Args>...);
  template<typename... Ts> Behavior& expects(detail::expect_t<Args>...);
  template<typename T> Behavior& returns(T&&);
  template<typename E> Behavior& throws(E&&);
  template<typename... SigArgs> Behavior& emits(
      void (Class::*)(SigArgs...),
      SigArgs&&...
    );

  // Set the exact number or a range of expected productions.
  //
  // @example: b.times(2, 4)  // Expect 2, 3 or 4 productions.
  Behavior& times(unsigned int);
  Behavior& times(unsigned int, unsigned int);

  // Configure this to be persistent/immortal. This overrides any
  // previous or future `times` calls.
  Behavior& persists();

  // Setters for is_tuple_pack_equal_.
  template<typename... Deriveds> Behavior& polymorphic();
  void setIsEqual(std::shared_ptr<detail::IWrapInSharedEqual<Args...>>);

  // Check if this is persistent (i.e. still has productions left or
  // persists).
  bool is_persistent() const;
  // Check if this persists or the expected number of productions has
  // been met.
  bool is_exhausted() const;

  // Check if the parameter pack matches the values stored in `expect_`.
  bool match(const Args&...) const;
  // Produce: return result_ or exception_. The default production
  // is nullptr (representing no production). exception_ is returned
  // if it is not null, otherwise result_ is returned.
  std::variant<Result, std::exception_ptr> produce();

private:
  std::optional<std::tuple<std::shared_ptr<ICompare<Args>>...>> expect_{};
  Result result_{};
  std::exception_ptr exception_{};
  unsigned int times_min_ = 1;
  unsigned int times_max_ = 1;
  unsigned int num_calls_ = 0;  // Number of productions made.
  bool persists_ = false;
  std::shared_ptr<detail::IWrapInSharedEqual<Args...>> wrap_in_shared_equal_{};
  detail::InvokeOnPack<std::tuple<Args...>> invoke_on_pack_{};
};

} // namespace

#include "Behavior.tpp"

#endif /* DRMOCK_SRC_MOCK_BEHAVIOR_H */
