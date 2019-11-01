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

#include "Warehouse.h"

namespace drmock { namespace samples {

void
Warehouse::add(
    std::string type, 
    std::size_t quantity
  )
{
  inventory_[std::move(type)] += quantity;
}

bool
Warehouse::remove(
    const std::string& type, 
    std::size_t quantity
  )
{
  auto a = inventory_[type];
  
  // If there is not enough of `type` available, return `false`.
  if (a < quantity)
  {
    return false;
  }

  // Otherwise, deduce `quantity` from the stash and return `true`.
  inventory_[type] -= quantity;
  return true;
}

}} // namespace drmock::samples
