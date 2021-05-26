#ifndef DRMOCK_SRC_MOCK_DETAIL_IINVOKE_H
#define DRMOCK_SRC_MOCK_DETAIL_IINVOKE_H

#include <memory>

namespace drmock { namespace detail {

template<typename T1, typename T2>
class IInvoke
{
public:
  virtual ~IInvoke() = default;

  virtual bool operator()(
      const T1&,
      const T2&
    ) const = 0;
};

}} // namespace drmock::detail

#endif /* DRMOCK_SRC_MOCK_DETAIL_IINVOKE_H */
