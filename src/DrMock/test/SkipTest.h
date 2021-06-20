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

#ifndef DRMOCK_SRC_TEST_SKIP_TEST_H
#define DRMOCK_SRC_TEST_SKIP_TEST_H

#include <stdexcept>
#include <string>

namespace drtest {

class SkipTest : public std::exception
{
public:
  SkipTest(std::string expr);

  const char* what() const noexcept override;

private:
  std::string what_{};
};

} // namespace drtest

#endif /* DRMOCK_SRC_TEST_SKIP_TEST_H */
