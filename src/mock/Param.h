#ifndef DRMOCK_SRC_MOCK_PARAM_H
#define DRMOCK_SRC_MOCK_PARAM_H

#include "IParam.h"

namespace drmock {

template<typename T>
class Param : public IParam<T>
{
public:
  Param(T&& value) : value_{std::move(value)}
  {}

  const T& get() const
  {
    return value_;
  }

private:
  T value_;
};

} // namespace drmock

#endif /* DRMOCK_SRC_MOCK_PARAM_H */
