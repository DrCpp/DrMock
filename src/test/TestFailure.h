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

/* FIXME 
 *
 * Put drmock::detail::StreamIfStreamable into a neutral namespace, e.g.
 * drutil::StreamIfStreamable; by extension, move TypeTraits and
 * TypeInfo, there as well.
 *
 * FIXME */

#ifndef DRMOCK_SRC_TEST_TESTFAILURE_H
#define DRMOCK_SRC_TEST_TESTFAILURE_H

#include <exception>
#include <sstream>
#include <string>

#include "../mock/detail/Diagnostics.h"

namespace drtest { namespace detail {

class TestFailure : public std::exception
{
public:
  TestFailure(int line, std::string expr);
  template<typename LhsType, typename RhsType> TestFailure(
      int line,
      std::string op,
      std::string lhs_expr,
      std::string rhs_expr,
      const LhsType& lhs,
      const RhsType& rhs
    );

  const char* what() const noexcept override;
  int line() const noexcept;

private:
  int line_ = 0;
  std::string what_{};
};

template<typename LhsType, typename RhsType>
TestFailure::TestFailure(
    int line,
    std::string op,
    std::string lhs_expr,
    std::string rhs_expr,
    const LhsType& lhs,
    const RhsType& rhs
  )
:
  TestFailure{line, {}}
{
  std::stringstream s{};
  s << std::endl;
  s << "    (" << std::move(lhs_expr) << ") " << std::endl;
  s << "      " << drmock::detail::StreamIfStreamable<LhsType>{lhs} << std::endl;
  s << "    (expected " << op << ")" << std::endl;
  s << "      " << drmock::detail::StreamIfStreamable<RhsType>{rhs} << std::endl;
  what_ = s.str();
}

}} // namespace drtest::detail

#endif /* DRMOCK_SRC_TEST_TESTFAILURE_H */
