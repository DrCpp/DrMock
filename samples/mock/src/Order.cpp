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
