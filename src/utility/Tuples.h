#ifndef DRMOCK_SRC_UTILITY_TUPLES_H
#define DRMOCK_SRC_UTILITY_TUPLES_H

namespace drutility {

template<typename... Ts>
using last_t = std::tuple_element_t<sizeof...(Ts) - 1, std::tuple<Ts...>>;

} // namespace drtest

#endif /* DRMOCK_SRC_UTILITY_TUPLES_H */
