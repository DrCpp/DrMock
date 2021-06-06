#ifndef DRMOCK_SRC_MOCK_DETAIL_WRAP_IN_SHARED_EQUAL_H
#define DRMOCK_SRC_MOCK_DETAIL_WRAP_IN_SHARED_EQUAL_H

#include "../Equal.h"
#include "../ICompare.h"

namespace drmock { namespace detail {

// Check if `T1` is a `ICompare<T2>`.
template<typename T1, typename T2>
struct is_compare
{
  static constexpr bool value = std::is_base_of_v<ICompare<T2>, T1>;
};

template<typename T1, typename T2>
inline constexpr bool is_compare_v = is_compare<T1, T2>::value;

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
  template<typename... Ts>
  std::tuple<std::shared_ptr<ICompare<Bases>>...>
  wrap(Ts&&... pack)
  {
    // TODO static_assert
    return std::make_tuple(wrap_in_shared_equal<Bases, Deriveds>(std::forward<Ts>(pack))...);
  }
};

}} // namespace drmock::detail

#endif /* DRMOCK_SRC_MOCK_DETAIL_WRAP_IN_SHARED_EQUAL_H */
