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

template<typename Base, typename Derived>
std::shared_ptr<Equal<Base, Derived>>
equal(Base expected)
{
  return std::make_shared<Equal<Base, Derived>>(std::move(expected));
}

} // namespace drmock

#endif /* DRMOCK_SRC_MOCK_EQUAL_H */
