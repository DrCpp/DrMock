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

#include "Controller.h"

#include <algorithm>

#include <DrMock/mock/IMethod.h>

namespace drmock {

Controller::Controller(std::vector<std::shared_ptr<IMethod>> methods)
:
  methods_{std::move(methods)}
{}

bool
Controller::verify() const
{
  return std::all_of(
      methods_.begin(), methods_.end(),
      [] (const auto& method) { return method->verify(); }
    );
}

std::string
Controller::makeFormattedErrorString() const
{
  std::string result = "";
  for (std::size_t i = 0; i < methods_.size(); ++i)
  {
    auto err = methods_[i]->makeFormattedErrorString();
    if (err == "")
    {
      continue;
    }
    result += err;

    if (i != methods_.size() - 1)
    {
      result += "\n";
    }
  }
  return result;
}

} // namespace drmock
