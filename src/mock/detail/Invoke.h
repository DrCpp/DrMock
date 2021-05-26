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
  > : public IInvoke<T>
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
//     typename std::enable_if<
//             is_shared_ptr<T>::value
//         and is_shared_ptr<U>::value
//         and std::is_base_of<typename T::element_type, typename U::element_type>::value
//         and not std::is_same<T, U>::value
//       >::type
//   > : public IIsEqual<T>
// {
//   using ElemType = typename U::element_type;
//   bool operator()(const std::shared_ptr<IInvocable<ElemType>>& lhs, const T& rhs) const override
//   {
//     std::shared_ptr<ElemType> lhs_cast = std::dynamic_pointer_cast<ElemType>(lhs);
//     std::shared_ptr<ElemType> rhs_cast = std::dynamic_pointer_cast<ElemType>(rhs);
//     return (
//         lhs_cast and rhs_cast and IsEqual<ElemType>{}(*lhs_cast, *rhs_cast)
//       );
//   }
// };


}} // namespace drmock::detail

#endif /* DRMOCK_SRC_MOCK_DETAIL_INVOKE_H */
