#ifndef DRMOCK_SRC_TEST_TAG_H
#define DRMOCK_SRC_TEST_TAG_H

namespace drtest { namespace tags {

enum Tag
{
  none = 0,
  skip = (1 << 0),
  xfail = (1 << 1)
};

}} // namespace drtest::tags

#endif /* DRMOCK_SRC_TEST_TAG_H */
