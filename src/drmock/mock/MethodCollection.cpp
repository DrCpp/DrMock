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

#include "MethodCollection.h"

#include <algorithm>

#include "IMethod.h"

namespace drmock {

MethodCollection::MethodCollection(std::vector<std::shared_ptr<IMethod>> methods)
:
  methods_{std::move(methods)}
{}

bool
MethodCollection::verify() const
{
  return std::all_of(
      methods_.begin(), methods_.end(),
      [] (const auto& method) { return method->verify(); }
    );
}

std::string
MethodCollection::makeFormattedErrorString() const
{
  std::string err = "";
  for (std::size_t i = 0; i < methods_.size(); ++i)
  {
    err += methods_[i]->makeFormattedErrorString();

    if (i != methods_.size() - 1)
    {
      err += "\n";
    }
  }
  return err;
}

} // namespace drmock
