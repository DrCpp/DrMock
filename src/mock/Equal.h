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

} // namespace drmock

#endif /* DRMOCK_SRC_MOCK_EQUAL_H */
