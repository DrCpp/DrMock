#include "SkipTest.h"

namespace drtest {

SkipTest::SkipTest(std::string expr)
:
  what_{std::move(expr)}
{}

const char*
SkipTest::what() const noexcept
{
  return what_.c_str();
}

} // namespace drtestßæ
