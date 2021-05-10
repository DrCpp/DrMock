#ifndef DRMOCK_SRC_TEST_COMPARE_H
#define DRMOCK_SRC_TEST_COMPARE_H

#include "../mock/IInvocable.h"

namespace drtest {

template<typename T>  // T may be any numerical type.
class AlmostEqual : public drmock::IInvocable<T>
{
public:
  AlmostEqual(T expected, double abs_tol, double rel_tol)
  :
    expected_{expected},
    abs_tol_{abs_tol},
    rel_tol_{rel_tol}
  {}

  virtual bool invoke(const T& t) const override
  {
    return abs(t - expected_) < abs_tol_ + rel_tol_*abs(expected_);
  }

private:
  T expected_;
  double abs_tol_;
  double rel_tol_;
};

} // namespace drtest

#endif /* DRMOCK_SRC_TEST_COMPARE_H */
