#include "Interface.h"

#include "SkipTest.h"

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

void
tagRow(const std::string& row, tags tag)
{
  drutility::Singleton<detail::Global>::get()->tagRow(row, tag);
}

void
skip()
{
  skip("");
}

void
skip(std::string what)
{
  throw SkipTest{what};
}

void
xfail()
{
  drutility::Singleton<detail::Global>::get()->xfail();
}

} // namespace drtest
