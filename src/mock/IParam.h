#ifndef DRMOCK_SRC_MOCK_IPARAM_H
#define DRMOCK_SRC_MOCK_IPARAM_H

namespace drmock {

template<typename T>
class IParam
{
public:
  virtual ~IParam() = default;
};

} // namespace drmock

#endif /* DRMOCK_SRC_MOCK_IPARAM_H */
