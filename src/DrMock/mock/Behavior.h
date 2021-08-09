/* This file is part of DrMock.
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

#ifndef DRMOCK_SRC_DRMOCK_MOCK_BEHAVIOR_H
#define DRMOCK_SRC_DRMOCK_MOCK_BEHAVIOR_H

#include <memory>
#include <optional>
#include <utility>
#include <variant>

#include <DrMock/mock/detail/MatchPack.h>
#include <DrMock/mock/detail/IMakeTupleOfMatchers.h>
#include <DrMock/mock/AbstractSignal.h>

namespace drmock {

/**
 * The smallest unit of a method's behavior in **DrMock**.
 *
 * See `AbstractBehavior` for details on template parameters. Note that
 * `Behavior` does not inherit from `AbstractBehavior`; `Behavior`
 * objects are used as smallest unit of behaviors in `BehaviorQueue`
 * objects.
 *
 * Every `Behavior` object can _expect_ a call with a specified _input_
 * (a set of arguments that match the parameter types), and _produce_ a
 * result on a call: Return a value of type `ReturnType` and/or emit a
 * Qt signal, _or_ an exception pointer (simulating a thrown exception).
 * Returning a value and emitting a signal may both occur, but
 * exceptions are exclusive.
 *
 * Furthermore, every `Behavior` object has a _life span_, which
 * means that it can only `produce()` a fixed number (but possibly
 * infinite) number of times. Afterwards, the `Behavior` objects no
 * longer _persists_.
 *
 * Making conflicting configurations calls (such as `Behavior::returns` followed
 * by `Behavior::throws`) will result in an error.
 *
 * Every `Behavior` has a member of type
 * `std::shared_ptr<detail::IMakeTupleOfMatchers<Args...>>`. The job of
 * this _matching handler_ is to wrap expected values passed to the
 * behavior via `expects` in `std::shared_ptr<IMatcher<Args>>...`
 * objects, which are used to check if an input matches the behaviors
 * expected input. By default, they are wrapped in
 * `std::shared_ptr<Equal<Args>>...` objects.
 *
 * If `Behavior::polymorphic<Deriveds...>()` is called, a new handler is created
 * which wraps expected values in
 * `std::shared_ptr<Equal<Args, Deriveds>>...` objects. This means that
 * when polymorphic objects are used in `call`, then they are matched
 * against the expected behaviors by comparing them as `Deriveds...`,
 * not as base types. Note that `Behavior::polymorphic<Deriveds...>()` must be
 * called _before_ calling `expects(input...)` in order to have an
 * effect.
 */
template<typename Class, typename ReturnType, typename... Args>
class Behavior
{
  using Result = std::pair<
      std::shared_ptr<std::decay_t<ReturnType>>,
      std::shared_ptr<AbstractSignal<Class>>
    >;

public:
  Behavior();
  /**
   * @param make_tuple_of_matchers The matching handler
   */
  Behavior(std::shared_ptr<detail::IMakeTupleOfMatchers<Args...>> make_tuple_of_matchers);

  /**
   * Expect _any_ input.
   */
  template<typename T = std::tuple<Args...>>
  std::enable_if_t<(std::tuple_size_v<T> > 0), Behavior&> expects();

  /**
   * @param args... The expected arguments or matchers
   *
   * For every `i=0, ..., sizeof...(Args)`, the ith component of
   * `args...` may be of type `Args[i]` (a _raw expected value_) or
   * `std::shared_ptr<IMatcher<Args[i]>>` (a matcher). Raw expected
   * values are wrapped in an
   * `std::shared_ptr<Equal<Args[i], Deriveds[i]>>` by the matching
   * handler. This allows the consumer to use custom matchers alongside
   * the standard equality matcher `Equal`.
   *
   * The result will be stored in the `expect_` member, a tuple of
   * matchers. When `match` is called, the input `args...` is matched
   * componentwise against the stored matchers.
   *
   * (The `Deriveds` parameter pack depends on the matching handler. It
   * may be set using the `Behavior::polymorphic` template method.)
   *
   */
  Behavior& expects(detail::expect_t<Args>...);

  /**
   * Call `polymorphic<Deriveds...>()` followed by `expects(args...)`.
   *
   * @tparam Deriveds... The expected polymorphic type of the expected
   *   values
   * @param args... The expected arguments or matchers
   *
   * See `Behavior::polymorphic` and `Behavior::expects` for details.
   */
  template<typename... Deriveds> Behavior& expects(detail::expect_t<Args>... args);

  /**
   * Configure `this` to return `result` on productions.
   */
  template<typename T> Behavior& returns(T&& result);

  /**
   * Configure `this` to return an `std::exception_ptr<E>` to
   * `exception`.
   */
  template<typename E> Behavior& throws(E&&);

  /**
   * Configure `this` to emit `signal` with `args...` as arguments.
   */
  template<typename... SigArgs> Behavior& emits(
      void (Class::*signal)(SigArgs...),
      SigArgs&&... args
    );

  /**
   * Set the exact number of expected productions.
   */
  Behavior& times(unsigned int);

  /**
   * Set a range of expected productions.
   *
   * @param min The minimum number of productions expected
   * @param max The maximum number of productions expected
   */
  Behavior& times(unsigned int min, unsigned int max);

  /**
   * Define `this` to be persistent.
   *
   * Note that this will override any _previous and future_ calls of
   * `Behavior::times`.
   */
  Behavior& persists();

  /**
   * Replace the matching handler with a new
   * `std::shared_ptr<detail::MakeTupleOfMatchers<std::tuple<Args...>, std::tuple<Deriveds...>>>`.
   *
   * This call will not change the expected value. Only the effect of future calls to
   * `Behavior::expect` is changed.
   */
  template<typename... Deriveds> Behavior& polymorphic();

  /**
   * Check if `this` is persistent.
   */
  bool is_persistent() const;

  /**
   * Check if `this` is persistent or the expected number of productions
   * has been achieved.
   */
  bool is_exhausted() const;

  /**
   * Match `args...` against the stored matchers.
   *
   * The matching is done componentwise (see `Behavior::expects` for details). If
   * no matchers are stored, always return `true`.
   */
  bool match(const Args&... args) const;

  /**
   * Produce a return value, Qt signal emit or exception pointer.
   *
   * The default production is `nullptr` (representing no value).
   */
  std::variant<Result, std::exception_ptr> produce();

private:
  std::optional<std::tuple<std::shared_ptr<IMatcher<Args>>...>> expect_{};
  Result result_{};
  std::exception_ptr exception_{};
  unsigned int times_min_ = 1;
  unsigned int times_max_ = 1;
  unsigned int num_calls_ = 0;  // Number of productions made.
  bool persists_ = false;
  std::shared_ptr<detail::IMakeTupleOfMatchers<Args...>> make_tuple_of_matchers_{};
  detail::MatchPack<std::tuple<Args...>> match_pack_{};
};

} // namespace

#include "Behavior.tpp"

#endif /* DRMOCK_SRC_DRMOCK_MOCK_BEHAVIOR_H */
