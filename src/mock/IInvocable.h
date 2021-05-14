#ifndef DRMOCK_SRC_MOCK_IINVOCABLE_H
#define DRMOCK_SRC_MOCK_IINVOCABLE_H

#include "IParam.h"

namespace drmock {

template<typename T, typename Result = bool>
class IInvocable : public IParam<T>
{
public:
  virtual ~IInvocable() = default;
  virtual Result invoke(const T&) const = 0;
};

} // namespace drmock

#endif /* DRMOCK_SRC_MOCK_IINVOCABLE_H */
