#include "Interface.h"

namespace drtest {

void
mark(Mark m)
{
  drutility::Singleton<detail::Global>::get()->mark(m);
}

void
skip()
{
  mark(Mark::Skip);
}

} // namespace drtest
