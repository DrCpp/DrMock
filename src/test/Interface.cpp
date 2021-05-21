#include "Interface.h"

#include "SkipTest.h"

namespace drtest {

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
