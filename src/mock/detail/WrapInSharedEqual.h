#ifndef DRMOCK_SRC_MOCK_DETAIL_WRAP_IN_SHARED_EQUAL_H
#define DRMOCK_SRC_MOCK_DETAIL_WRAP_IN_SHARED_EQUAL_H

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
inline constexpr bool is_compare_v = is_compare<T, Bsae>::value;

template<typename Base, typename Derived, typename T>
std::enable_if_t<is_compare_v<T, Base>, std::shared_ptr<ICompare<Base>>>
wrap_in_shared_equal(const std::shared_ptr<T>& ptr)
{
  return ptr;
}

template<typename Base, typename Derived>
std::shared_ptr<ICompare<Base>>
wrap_in_shared_equal(Base t)
{
  return std::make_shared<Equal<Base, Derived>>(std::move(t));
}

template<typename Base, typename Derived = Base>
struct WrapInSharedEqual;

template<typename... Bases, typename... Deriveds>
struct WrapInSharedEqual<std::tuple<Bases...>, std::tuple<Deriveds...>>
{
  static_assert(sizeof...(Bases) == sizeof...(Deriveds));

  template<typename... Ts>
  std::tuple<std::shared_ptr<ICompare<Bases>>...>
  wrap(Ts&&... pack)
  {
    static_assert(sizeof...(Ts) == sizeof...(Deriveds));
    return std::make_tuple(wrap_in_shared_equal<Bases, Deriveds>(std::forward<Ts>(pack))...);
  }
};

}} // namespace drmock::detail

#endif /* DRMOCK_SRC_MOCK_DETAIL_WRAP_IN_SHARED_EQUAL_H */
