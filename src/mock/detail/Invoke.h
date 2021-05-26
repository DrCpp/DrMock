#ifndef DRMOCK_SRC_MOCK_DETAIL_INVOKE_H
#define DRMOCK_SRC_MOCK_DETAIL_INVOKE_H

#include <type_traits>

#include "IInvoke.h"
#include "TypeTraits.h"

namespace drmock { namespace detail {

template<typename T, typename U = T, typename enable = void>
struct Invoke;

template<typename T>
struct Invoke<
    T,
    T,
    std::enable_if_t<not is_tuple<T>::value>
  > : public IInvoke<std::shared_ptr<IInvocable<T>>, T>
{
  bool operator()(const std::shared_ptr<IInvocable<T>>& f, const T& t) const override
  {
    return f->invoke(t);
  }
};

// template<typename T, typename U>
// struct Invoke<
//     T,
//     U,
//     typename std::enable_if_t<
//         is_tuple<T>::value and is_tuple<U>::value
//         and (std::tuple_size<T>::value == std::tuple_size<U>::value)
//       >::type
//   > : public IIsEqual<T>
// {
// public:
//   bool operator()(const T& lhs, const T& rhs) const override
//   {
//     return impl(
//         lhs,
//         rhs,
//         std::make_index_sequence<std::tuple_size<T>::value>{}
//       );
//   }
// 
// private:
//   // Implementation detail of `operator()(const T&, const T&)`.
//   template<std::size_t... Is>
//   static bool impl(const T& lhs, const T& rhs, std::index_sequence<Is...>)
//   {
//     return (... and IsEqual<
//             typename std::tuple_element<Is, T>::type,
//             typename std::tuple_element<Is, U>::type
//           >{}(std::get<Is>(lhs), std::get<Is>(rhs))
//       );
//   }
// };



}} // namespace drmock::detail

#endif /* DRMOCK_SRC_MOCK_DETAIL_INVOKE_H */
