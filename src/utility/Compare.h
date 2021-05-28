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

#ifndef DRMOCK_SRC_UTILITY_COMPARE_H
#define DRMOCK_SRC_UTILITY_COMPARE_H

// Set global default for absolute and relative tolerance. All tolerance
// specified in double.
#ifndef DRTEST_ABS_TOL
#define DRTEST_ABS_TOL 1e-06  // double
#endif
#ifndef DRTEST_REL_TOL
#define DRTEST_REL_TOL 1e-06  // double
#endif

namespace drutility {

template<typename T> bool almost_equal(T actual, T expected, T abs_tol, T rel_tol);
template<typename T> bool almost_equal(T actual, T expected);

} // namespace drutility

#include "Compare.tpp"

#endif /* DRMOCK_SRC_UTILITY_COMPARE_H */
