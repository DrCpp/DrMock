#ifndef DRMOCK_SRC_MOCK_ABSTRACTSIGNAL_H
#define DRMOCK_SRC_MOCK_ABSTRACTSIGNAL_H

namespace drmock {

template<typename Parent>
class AbstractSignal
{
public:
  virtual ~AbstractSignal() = default;

  virtual void invoke(Parent*) = 0;
};

} // namespace drmock

#endif /* DRMOCK_SRC_MOCK_ABSTRACTSIGNAL_H */
