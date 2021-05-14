#ifndef DRMOCK_SRC_MOCK_IEXPECT_H
#define DRMOCK_SRC_MOCK_IEXPECT_H

namespace drmock {

template<typename T, typename Result = bool>
class IExpect
{
public:
  virtual ~IExpect() = default;
  virtual Result invoke(const T&) const = 0;
};

} // namespace drmock

#endif /* DRMOCK_SRC_MOCK_IEXPECT_H */
