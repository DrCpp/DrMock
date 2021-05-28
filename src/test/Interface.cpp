#include "Interface.h"

namespace drtest {

void
abs_tol(double value)
{
  drutility::Singleton<detail::Global>::get()->abs_tol(value);
}

void
rel_tol(double value)
{
  drutility::Singleton<detail::Global>::get()->rel_tol(value);
}

} // namespace drtest
