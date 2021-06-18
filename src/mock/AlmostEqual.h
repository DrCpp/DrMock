#ifndef DRMOCK_SRC_MOCK_ALMOST_EQUAL_H
#define DRMOCK_SRC_MOCK_ALMOST_EQUAL_H

#include "ICompare.h"
#include "../utility/Compare.h"

namespace drmock {

template<typename T>
class AlmostEqual : public ICompare<T>
{
public:
  AlmostEqual(T expected)
  :
    AlmostEqual(expected, DRTEST_ABS_TOL, DRTEST_REL_TOL)
  {}
  AlmostEqual(T expected, T abs_tol, T rel_tol)
  :
    expected_{expected}, abs_tol_{abs_tol}, rel_tol_{rel_tol}
  {}

  bool invoke(const T& actual) const override
  {
    return drutility::almost_equal(actual, expected_, abs_tol_, rel_tol_);
  }

private:
  T expected_;
  T abs_tol_;
  T rel_tol_;
};

// Convenience function for quickly creating a shared object.
template<typename T>
std::shared_ptr<ICompare<T>>
almost_equal(T expected)
{
  return std::make_shared<AlmostEqual<T>>(expected);
}

template<typename T>
std::shared_ptr<ICompare<T>>
almost_equal(T expected, T abs_tol, T rel_tol)
{
  return std::make_shared<AlmostEqual<T>>(expected, abs_tol, rel_tol);
}

} // namespace drmock

#endif /* DRMOCK_SRC_MOCK_ALMOST_EQUAL_H */
