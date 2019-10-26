#ifndef DRMOCK_SAMPLES_MOCKING_SRC_ORDER_H
#define DRMOCK_SAMPLES_MOCKING_SRC_ORDER_H

#include <memory>
#include <string>

#include "IWarehouse.h"

namespace drmock { namespace samples {

class Order
{
public:
  Order(std::string commodity, std::size_t quantity);

  void fill(const std::shared_ptr<IWarehouse>&);
  bool filled() const;

private:
  bool filled_;
  std::string commodity_;
  std::size_t quantity_;
};

}} // drmock::samples

#endif /* DRMOCK_SAMPLES_MOCKING_SRC_ORDER_H */
