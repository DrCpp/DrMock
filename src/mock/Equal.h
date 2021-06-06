#ifndef DRMOCK_SRC_MOCK_EQUAL_H
#define DRMOCK_SRC_MOCK_EQUAL_H

#include "ICompare.h"
#include "detail/IsEqual.h"

namespace drmock {

template<typename Base, typename Derived = Base>
class Equal : public ICompare<Base>
{
public:
  Equal(Base expected)
  :
    expected_{std::move(expected)}
  {}

  bool
  invoke(const Base& actual) const override
  {
    auto is_equal = detail::IsEqual<Base, Derived>{};
    return is_equal(expected_, actual);
  }

private:
  Base expected_;
};

template<typename T1, typename T2>
std::enable_if_t<is_compare_v<T2, T1>, std::shared_ptr<ICompare<T1>>>
wrap_in_shared_equal(const std::shared_ptr<T2>& ptr)
{
  return ptr;
}

template<typename T1>
std::shared_ptr<ICompare<T1>>
wrap_in_shared_equal(T1&& t)
{
  return std::make_shared<Equal<T1>>(std::forward<T1>(t));
}

} // namespace drmock

#endif /* DRMOCK_SRC_MOCK_EQUAL_H */
