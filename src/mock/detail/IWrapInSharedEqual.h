#ifndef DRMOCK_SRC_MOCK_DETAIL_IWRAPPINSHAREDEQUAL_H
#define DRMOCK_SRC_MOCK_DETAIL_IWRAPPINSHAREDEQUAL_H

#include <memory>
#include <tuple>
#include <variant>

#include "../ICompare.h"

namespace drmock { namespace detail {

template<typename T>
using expect_t = std::variant<T, std::shared_ptr<ICompare<T>>>;

template<typename... Bases>
class IWrapInSharedEqual
{
public:
  virtual ~IWrapInSharedEqual() = default;
  virtual std::tuple<std::shared_ptr<ICompare<Bases>>...>
  wrap(expect_t<Bases>&&... pack) = 0;
};

}} // namespace drmock::detail

#endif /* DRMOCK_SRC_MOCK_DETAIL_IWRAPPINSHAREDEQUAL_H */
