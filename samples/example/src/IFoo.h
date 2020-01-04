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

#ifndef DRMOCK_SAMPLES_EXAMPLES_SRC_IFOO_H
#define DRMOCK_SAMPLES_EXAMPLES_SRC_IFOO_H

#include <string>

namespace drmock { namespace samples {

class IFoo
{
public:
  virtual ~IFoo() = default;

  virtual int f(std::string, unsigned int) = 0;
  virtual float g() = 0;
  virtual void h() = 0;
};

}} // namespace drmock::samples

#endif /* DRMOCK_SAMPLES_EXAMPLES_SRC_IFOO_H */
