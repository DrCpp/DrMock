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

#include "Order.h"

namespace drmock { namespace samples {

Order::Order(std::string commodity, std::size_t quantity)
:
  commodity_{std::move(commodity)},
  quantity_{quantity}
{}

void
Order::fill(const std::shared_ptr<IWarehouse>& wh)
{
  filled_ = wh->remove(commodity_, quantity_);
}

bool
Order::filled() const
{
  return filled_;
}

}} // drmock::samples
