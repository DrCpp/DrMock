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
  DRTEST_ASSERT(warehouse->mock.verify());
  // Check that the return value of `filled` is correct.
  DRTEST_COMPARE(order.filled(), true);
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
  DRTEST_ASSERT(warehouse->mock.verify());
  // Check that the return value of `filled` is correct.
  DRTEST_COMPARE(order.filled(), false);
}
