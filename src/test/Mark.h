#ifndef DRMOCK_SRC_TEST_MARK_H
#define DRMOCK_SRC_TEST_MARK_H

#include <type_traits>

namespace drtest {

enum Mark : unsigned int
{
  None = 0,
  Skip = 1 << 0,
  Xfail = 1 << 1
};

inline Mark
operator|(Mark lhs, Mark rhs)
{
  return static_cast<Mark>(
      static_cast<std::underlying_type_t<Mark>>(lhs) |
      static_cast<std::underlying_type_t<Mark>>(rhs)
    );
}

inline Mark
operator&(Mark lhs, Mark rhs)
{
  return static_cast<Mark>(
      static_cast<std::underlying_type_t<Mark>>(lhs) &
      static_cast<std::underlying_type_t<Mark>>(rhs)
    );
}

inline Mark&
operator|=(Mark& lhs, Mark rhs)
{
  lhs = lhs | rhs;
  return lhs;
}

inline Mark&
operator&=(Mark& lhs, Mark rhs)
{
  lhs = lhs & rhs;
  return lhs;
}

} // namespace drtest

#endif /* DRMOCK_SRC_TEST_MARK_H */
