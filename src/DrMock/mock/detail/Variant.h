#ifndef DRMOCK_SRC_DRMOCK_MOCK_DETAIL_VARIANT_H
#define DRMOCK_SRC_DRMOCK_MOCK_DETAIL_VARIANT_H

#include <type_traits>
#include <utility>
#include <variant>

namespace drmock { namespace detail {

// Variant wrapper which allows implicit conversions for the first
// alternative of two.
template<typename T1, typename T2>
class Variant
{
public:
  Variant() : var_{T1{}}
  {}

  template<typename... Ts, typename = std::void_t<decltype(T1{std::declval<Ts>()...})>>
  Variant(Ts&&... ts)
  :
    var_{T1{std::forward<Ts>(ts)...}}  // Need this indirection for implicit conversion.
  {}

  Variant(T1 t) : var_{std::move(t)}
  {
  }

  template<typename T, typename = std::enable_if_t<not std::is_constructible_v<T1, T>>>
  Variant(T t) : var_{std::move(t)}
  {
  }

  template<typename T>
  Variant&
  operator=(const T& rhs)
  {
    var_ = rhs;
  }

  template<typename T>
  Variant&
  operator=(T&& rhs)
  {
    var_ = std::move(rhs);
  }

  template<typename T>
  bool
  holds_alternative() const
  {
    return std::holds_alternative<T>(var_);
  }

  template<typename T>
  T&
  get() &
  {
    return std::get<T>(var_);
  }

  template<typename T>
  T&&
  get() &&
  {
    return std::move(std::get<T>(var_));
  }

private:
  std::variant<T1, T2> var_;
};

}} // namespace drmock::detail

#endif /* DRMOCK_SRC_DRMOCK_MOCK_DETAIL_VARIANT_H */
