#ifndef DRMOCK_SRC_MOCK_DETAIL_WRAP_IN_SHARED_EQUAL_H
#define DRMOCK_SRC_MOCK_DETAIL_WRAP_IN_SHARED_EQUAL_H

#include <memory>
#include <tuple>
#include <type_traits>

#include "IWrapInSharedEqual.h"
#include "../Equal.h"
#include "../ICompare.h"

namespace drmock { namespace detail {

// Check if `T` is a `ICompare<Base>`.
template<typename T, typename Base>
struct is_compare
{
  static constexpr bool value = std::is_base_of_v<ICompare<Base>, T>;
};

template<typename T, typename Base>
inline constexpr bool is_compare_v = is_compare<T, Base>::value;

template<typename Base, typename Derived = Base>
std::shared_ptr<ICompare<Base>>
wrap_in_shared_equal(expect_t<Base>&& var)
{
  if (std::holds_alternative<Base>(var))
  {
    return std::make_shared<Equal<Base, Derived>>(std::get<Base>(var));
  }
  else
  {
    return std::get<std::shared_ptr<ICompare<Base>>>(var);
  }
}

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
};

}} // namespace drmock::detail

#endif /* DRMOCK_SRC_MOCK_DETAIL_WRAP_IN_SHARED_EQUAL_H */
