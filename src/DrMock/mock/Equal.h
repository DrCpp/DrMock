/* Copyright 2021 Ole Kliemann, Malte Kliemann
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

#ifndef DRMOCK_SRC_DRMOCK_MOCK_EQUAL_H
#define DRMOCK_SRC_DRMOCK_MOCK_EQUAL_H

#include <DrMock/mock/IMatcher.h>
#include <DrMock/mock/detail/IsEqual.h>

namespace drmock {

/**
 * For matching elements using equality.
 *
 * @tparam Base Base type of matched elements
 * @tparam Derived Derived type of matched elements (matters only for
 *   pointer types)
 *
 * `Derived` need not derive from `Base`. Instead, `(Base, Derived)`
 * **must** satisfy one of the following requirements (such a pair is
 * called _comparable_):
 *
 * 1. `Derived` is not abstract, inherits from `Base` and implements
 *    `bool operator==(const Derived&) const`
 * 2. `Base` is a `shared_ptr<T>` (or `unique_ptr<T>`) and `Derived` is a
 *    `shared_ptr<U>` (or `unique_ptr<U>`), and `(T, U)` satisfies 1., 2. or 3.
 * 3. `Base` is a `tuple<Ts...>` and `Derived` is `tuple<Us...>` of the
 *    same length `n` so that for all `i=0, ..., n-1`,
 *    `(Ts[i], Us[i])` satisfies 1., 2. or 3.
 */
template<typename Base, typename Derived = Base>
class Equal : public IMatcher<Base>
{
public:
  Equal(const Base& expected)
  :
    expected_{expected}
  {}

  Equal(Base&& expected)
  :
    expected_{std::move(expected)}
  {}

  /**
   * Check if `actual` is equal to `expected_`.
   *
   * In detail, this returns the following:
   *
   * 1. If `Base` and `Derived` are pointer types, then return `true` if
   *    `*expected_` and `*actual` are recursively equal as instances of
   *    the pointee type of `Derived`, otherwise `false`.
   * 2. If `Base` is an `std::tuple<Ts...>`, then return if `expected_`
   *    and `actual` are componentwise equal.
   * 3. Otherwise, return `expected_ == actual`.
   */
  bool
  match(const Base& actual) const override
  {
    auto is_equal = detail::IsEqual<Base, Derived>{};
    return is_equal(expected_, actual);
  }

private:
  Base expected_;  /**< The element to match against */
};

/**
 * Conveniently create a shared `Equal` object.
 *
 * See `Equal::Equal` for details.
 */
template<typename Base, typename Derived = Base>
std::shared_ptr<Equal<Base, Derived>>
equal(Base expected)
{
  return std::make_shared<Equal<Base, Derived>>(std::move(expected));
}

} // namespace drmock

#endif /* DRMOCK_SRC_DRMOCK_MOCK_EQUAL_H */
