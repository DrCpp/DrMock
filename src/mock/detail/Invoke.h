#ifndef DRMOCK_SRC_MOCK_DETAIL_INVOKE_H
#define DRMOCK_SRC_MOCK_DETAIL_INVOKE_H

#include <type_traits>

#include "IInvoke.h"
#include "TypeTraits.h"

namespace drmock { namespace detail {

template<typename T, typename U, typename enable = void>
struct Invoke;

template<typename T, typename U>
struct Invoke<
    T,
    U,
    std::enable_if_t<std::is_same_v<T, std::shared_ptr<IInvocable<U>>>>
  > : public IInvoke<std::shared_ptr<IInvocable<U>>, U>
{
  bool operator()(const std::shared_ptr<IInvocable<U>>& f, const U& t) const override
  {
    return f->invoke(t);
  }
};

template<typename T, typename U>
struct Invoke<
    T,
    U,
    std::enable_if_t<is_tuple<T>::value and is_tuple<U>::value>
  > : public IInvoke<T, U>
{
public:
  bool operator()(const T& lhs, const U& rhs) const override
  {
    return impl(
        lhs,
        rhs,
        std::make_index_sequence<std::tuple_size_v<T>>{}
      );
  }

private:
  // Implementation detail of `operator()(const T&, const T&)`.
  template<std::size_t... Is>
  static bool impl(const T& lhs, const U& rhs, std::index_sequence<Is...>)
  {
    return (Invoke<
            std::tuple_element_t<Is, T>,
            std::tuple_element_t<Is, U>
          >{}(std::get<Is>(lhs), std::get<Is>(rhs)) and ...
      );
  }
};



}} // namespace drmock::detail

#endif /* DRMOCK_SRC_MOCK_DETAIL_INVOKE_H */
