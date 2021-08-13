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

#include "DrMock/Test.h"
#include "mock/WarehouseMock.h"
#include "Order.h"

DRTEST_TEST(success)
{
  drmock::samples::Order order{"foo", 2};

  auto warehouse = std::make_shared<drmock::samples::WarehouseMock>();
  // Inform `warehouse` that it should expect an order of two units of
  // `"foo"` and should return `true` (indicating that those units are
  // available).
  warehouse->mock.remove().push()
      .expects("foo", 2)  // Expected arguments.
      .times(1)  // Expect **one** call only.
      .returns(true);  // Return value.

  order.fill(warehouse);
  // Check that `remove` was called with the correct arguments.
  DRTEST_ASSERT(warehouse->mock.control.verify());
  // Check that the return value of `filled` is correct.
  DRTEST_ASSERT(order.filled());
}

DRTEST_TEST(failure)
{
  drmock::samples::Order order{"foo", 2};

  auto warehouse = std::make_shared<drmock::samples::WarehouseMock>();
  // Inform `warehouse` that it should expect an order of two units of
  // `"foo"` and should return `false` (indicating that those units are
  // not available).
  warehouse->mock.remove().push()
      .expects("foo", 2)  // Expected arguments.
      .times(1)  // Expect **one** call only.
      .returns(false);  // Return value.

  order.fill(warehouse);
  // Check that `remove` was called with the correct arguments.
  DRTEST_ASSERT(warehouse->mock.control.verify());
  // Check that the return value of `filled` is correct.
  DRTEST_ASSERT(not order.filled());
}
