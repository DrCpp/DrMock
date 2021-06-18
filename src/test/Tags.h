#ifndef DRMOCK_SRC_TEST_TAG_H
#define DRMOCK_SRC_TEST_TAG_H

namespace drtest {

enum class tags
{
  none = 0,
  skip = (1 << 0),
  xfail = (1 << 1)
};

inline tags
operator|(tags lhs, tags rhs)
{
  return static_cast<tags>(
      static_cast<std::underlying_type_t<tags>>(lhs) |
      static_cast<std::underlying_type_t<tags>>(rhs)
    );
}

inline tags
operator&(tags lhs, tags rhs)
{
  return static_cast<tags>(
      static_cast<std::underlying_type_t<tags>>(lhs) &
      static_cast<std::underlying_type_t<tags>>(rhs)
    );
}

inline tags&
operator|=(tags& lhs, tags rhs)
{
  lhs = lhs | rhs;
  return lhs;
}

inline tags&
operator&=(tags& lhs, tags rhs)
{
  lhs = lhs & rhs;
  return lhs;
}


} // namespace drtest

#endif /* DRMOCK_SRC_TEST_TAG_H */
