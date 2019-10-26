#ifndef DRMOCK_SAMPLES_MOCKING_SRC_IWAREHOUSE_H
#define DRMOCK_SAMPLES_MOCKING_SRC_IWAREHOUSE_H

#include <string>

namespace drmock { namespace samples {

class IWarehouse
{
public:
  virtual ~IWarehouse() = default;

  virtual void add(
      std::string type, 
      std::size_t quantity
    ) = 0;
  virtual bool remove(
      const std::string& type,
      std::size_t quantity
    ) = 0;
};

}} // namespace drmock::samples

#endif /* DRMOCK_SAMPLES_MOCKING_SRC_IWAREHOUSE_H */
