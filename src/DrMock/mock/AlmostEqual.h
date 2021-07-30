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

#ifndef DRMOCK_SRC_DRMOCK_MOCK_ALMOSTEQUAL_H
#define DRMOCK_SRC_DRMOCK_MOCK_ALMOSTEQUAL_H

#include <memory>

#include <DrMock/mock/IMatcher.h>
#include <DrMock/utility/Compare.h>

namespace drmock {

/**
 * For matching floating-point numbers.
 *
 * @tparam T The type of floating-point number to compare
 *
 * The actual input matches the expected value if the following is true
 * (vertical bars denote absolute value):
 *
 * ```
 * |actual - expected| <= abs_tol + rel_tol*|expected|
 * ```
 */
template<typename T>
class AlmostEqual : public IMatcher<T>
{
public:
  /**
   * Specify the expected floating-point number.
   *
   * @param expected The value to match against
   *
   * The absolute and relative tolerance are the `DRTEST_*_TOL` default
   * values.
   */
  AlmostEqual(T expected)
  :
    AlmostEqual{expected, DRTEST_ABS_TOL, DRTEST_REL_TOL}
  {}

  /**
   * Specify the expected floating-point number, and the absolute and
   * relative tolerance.
   *
   * @param expected The value to match against
   * @param abs_tol The absolute tolerance
   * @param rel_tol The relative tolerance
   */
  AlmostEqual(T expected, T abs_tol, T rel_tol)
  :
    expected_{expected}, abs_tol_{abs_tol}, rel_tol_{rel_tol}
  {}

  /**
   * Check if the following holds:
   *
   * ```
   * |actual - expected_| <= abs_tol + rel_tol*|expected_|
   * ```
   */
  bool match(const T& actual) const override
  {
    return drutility::almost_equal(actual, expected_, abs_tol_, rel_tol_);
  }

private:
  T expected_;
  T abs_tol_;
  T rel_tol_;
};

/**
 * Convenience functions for quickly creating a shared `AlmostEqual`
 * object.
 *
 * See `AlmostEqual::AlmostEqual` for details.
 */
template<typename T>
std::shared_ptr<IMatcher<T>>
almost_equal(T expected)
{
  return std::make_shared<AlmostEqual<T>>(expected);
}

/**
 * Convenience functions for quickly creating a shared `AlmostEqual`
 * object.
 *
 * See `AlmostEqual::AlmostEqual` for details.
 */
template<typename T>
std::shared_ptr<IMatcher<T>>
almost_equal(T expected, T abs_tol, T rel_tol)
{
  return std::make_shared<AlmostEqual<T>>(expected, abs_tol, rel_tol);
}

} // namespace drmock

#endif /* DRMOCK_SRC_DRMOCK_MOCK_ALMOSTEQUAL_H */
