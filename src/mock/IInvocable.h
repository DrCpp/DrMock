#ifndef DRMOCK_SRC_MOCK_IINVOCABLE_H
#define DRMOCK_SRC_MOCK_IINVOCABLE_H

namespace drmock {

template<typename T>
class IInvocable
{
public:
  virtual ~IInvocable() = default;
  virtual bool invoke(const T&) const = 0;
};

} // namespace drmock

#endif /* DRMOCK_SRC_MOCK_IINVOCABLE_H */
