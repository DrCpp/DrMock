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

template<typename T>
class AlmostEqual : public IMatcher<T>
{
public:
  AlmostEqual(T expected)
  :
    AlmostEqual(expected, DRTEST_ABS_TOL, DRTEST_REL_TOL)
  {}
  AlmostEqual(T expected, T abs_tol, T rel_tol)
  :
    expected_{expected}, abs_tol_{abs_tol}, rel_tol_{rel_tol}
  {}

  bool match(const T& actual) const override
  {
    return drutility::almost_equal(actual, expected_, abs_tol_, rel_tol_);
  }

private:
  T expected_;
  T abs_tol_;
  T rel_tol_;
};

// Convenience function for quickly creating a shared object.
template<typename T>
std::shared_ptr<IMatcher<T>>
almost_equal(T expected)
{
  return std::make_shared<AlmostEqual<T>>(expected);
}

template<typename T>
std::shared_ptr<IMatcher<T>>
almost_equal(T expected, T abs_tol, T rel_tol)
{
  return std::make_shared<AlmostEqual<T>>(expected, abs_tol, rel_tol);
}

} // namespace drmock

#endif /* DRMOCK_SRC_DRMOCK_MOCK_ALMOSTEQUAL_H */
