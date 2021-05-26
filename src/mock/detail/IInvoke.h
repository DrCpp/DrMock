#ifndef DRMOCK_SRC_MOCK_DETAIL_IINVOKE_H
#define DRMOCK_SRC_MOCK_DETAIL_IINVOKE_H

#include <memory>

namespace drmock { namespace detail {

template<typename T>
class IInvoke
{
public:
  virtual ~IInvoke() = default;

  virtual bool operator()(
      const std::shared_ptr<IInvocable<T>>&,
      const T&
    ) const = 0;
};

}} // namespace drmock::detail

#endif /* DRMOCK_SRC_MOCK_DETAIL_IINVOKE_H */
