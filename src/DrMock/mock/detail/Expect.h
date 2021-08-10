#ifndef DRMOCK_SRC_DRMOCK_MOCK_DETAIL_EXPECT_H
#define DRMOCK_SRC_DRMOCK_MOCK_DETAIL_EXPECT_H

#include <memory>

#include <DrMock/mock/IMatcher.h>
#include <DrMock/mock/detail/Variant.h>

namespace drmock { namespace detail {

template<typename T>
using Expect = Variant<T, std::shared_ptr<IMatcher<T>>>;

}} // namespace drmock::detail

#endif /* DRMOCK_SRC_DRMOCK_MOCK_DETAIL_EXPECT_H */
