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

#ifndef DRMOCK_TESTS_INTEGRATION_IACCESSSPEC_H
#define DRMOCK_TESTS_INTEGRATION_IACCESSSPEC_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace outer { namespace inner {

class IAccessSpec
{
public:
  virtual ~IAccessSpec() = default;

  virtual void publicFunc() = 0;

protected:
  virtual void protectedFunc() = 0;

private:
  virtual void privateFunc() = 0;
};

}} // namespace outer::inner

#endif /* DRMOCK_TESTS_INTEGRATION_IACCESSSPEC_H */
