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

#ifndef DRMOCK_TESTS_INTEGRATION_IFUNCNOTABSTRACT_H
#define DRMOCK_TESTS_INTEGRATION_IFUNCNOTABSTRACT_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace outer { namespace inner {

class IFuncNotAbstract
{
public:
  IFuncNotAbstract() = default;

  virtual void fParameters(int, std::vector<float>)
  {
  }

  virtual std::vector<float> gParameters(
      const std::shared_ptr<std::unordered_map<int, std::string>>& test_name,
      float,
      std::string
    )
  {
    return {1.0f, 2.0f};
  }

  virtual int virtualMethodNotPure()
  {
    return 12;
  }

  int notVirtual()
  {
    return 12;
  }
};

}} // namespace outer::inner

#endif /* DRMOCK_TESTS_INTEGRATION_IFUNCNOTABSTRACT_H */
