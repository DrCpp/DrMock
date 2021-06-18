#ifndef DRMOCK_SRC_MOCK_DETAIL_WRAP_IN_SHARED_EQUAL_H
#define DRMOCK_SRC_MOCK_DETAIL_WRAP_IN_SHARED_EQUAL_H

#include <memory>
#include <tuple>
#include <type_traits>

#include "IWrapInSharedEqual.h"
#include "../Equal.h"
#include "../ICompare.h"

namespace drmock { namespace detail {

template<typename Base, typename Derived = Base>
struct WrapInSharedEqual;

template<typename... Bases, typename... Deriveds>
struct WrapInSharedEqual<std::tuple<Bases...>, std::tuple<Deriveds...>> : public IWrapInSharedEqual<Bases...>
{
  static_assert(sizeof...(Bases) == sizeof...(Deriveds));

  std::tuple<std::shared_ptr<ICompare<Bases>>...>
  wrap(expect_t<Bases>&&... pack)
  {
    return std::make_tuple(wrap_in_shared_equal<Bases, Deriveds>(std::forward<expect_t<Bases>>(pack))...);
  }

private:
  template<typename Base, typename Derived = Base>
  std::shared_ptr<ICompare<Base>>
  wrap_in_shared_equal(expect_t<Base>&& var)
  {
    if (std::holds_alternative<Base>(var))
    {
      return std::make_shared<Equal<Base, Derived>>(std::get<Base>(std::forward<expect_t<Base>>(var)));
    }
    else
    {
      return std::get<std::shared_ptr<ICompare<Base>>>(std::forward<expect_t<Base>>(var));
    }
  }
};

}} // namespace drmock::detail

#endif /* DRMOCK_SRC_MOCK_DETAIL_WRAP_IN_SHARED_EQUAL_H */
