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
 * `T` must be a floating-point type, in other words:
 * `std::is_floating_point_v<T>` must be true.
 *
 * The actual input matches the expected value if the following is true
 * (vertical bars denote absolute value):
 *
 * ```
 * |actual - expected_| <= abs_tol + rel_tol*|expected_|
 * ```
 */
template<typename T>
class AlmostEqual : public IMatcher<T>
{
public:
  /**
   * Specify the expected floating-point number.
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
  T expected_;  /**< The expected result */
  T abs_tol_;  /**< The absolute tolerance */
  T rel_tol_;  /**< The relative tolerance */
};

/**
 * Convenience functions for quickly creating a shared `AlmostEqual`
 * object.
 *
 * `T` must be a floating-point type, in other words:
 * `std::is_floating_point_v<T>` must be true.
 *
 * The absolute and relative tolerance are the `DRTEST_*_TOL` default
 * values.
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
 * `T` must be a floating-point type, in other words:
 * `std::is_floating_point_v<T>` must be true.
 */
template<typename T>
std::shared_ptr<IMatcher<T>>
almost_equal(T expected, T abs_tol, T rel_tol)
{
  return std::make_shared<AlmostEqual<T>>(expected, abs_tol, rel_tol);
}

} // namespace drmock

#endif /* DRMOCK_SRC_DRMOCK_MOCK_ALMOSTEQUAL_H */
