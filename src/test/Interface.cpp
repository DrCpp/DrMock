#include "Interface.h"

#include "SkipTest.h"

namespace drtest {

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
