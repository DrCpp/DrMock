#ifndef DRMOCK_SAMPLES_MOCKING_SRC_WAREHOUSE_H
#define DRMOCK_SAMPLES_MOCKING_SRC_WAREHOUSE_H

#include <map>

#include "IWarehouse.h"

namespace drmock { namespace samples {

class Warehouse : public IWarehouse
{
public:
  void add(std::string commodity, std::size_t quantity) override final;
  bool remove(const std::string& commodity, std::size_t quantity) override final;

private:
  std::map<std::string, std::size_t> inventory_;
};

}} // namespace drmock::samples

#endif /* DRMOCK_SAMPLES_MOCKING_SRC_WAREHOUSE_H */
