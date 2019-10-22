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

#include "StateObject.h"

namespace drmock {

std::string
StateObject::get() const
{
  return get("");
}

std::string
StateObject::get(const std::string& slot) const
{
  return slots_[slot];
}

void
StateObject::set(std::string state)
{
  set("", std::move(state));
}

void
StateObject::set(const std::string& slot, std::string state)
{
  slots_[slot] = std::move(state);
}

} // namespace drmock
