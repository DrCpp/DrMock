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
