/* Copyright 2021 Ole Kliemann, Malte Kliemann
 *
 * This file is part of DrMock.
 *
 * DrMock is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * DrMock is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with DrMock.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef DRMOCK_SRC_DRMOCK_TEST_TAGS_H
#define DRMOCK_SRC_DRMOCK_TEST_TAGS_H

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

#endif /* DRMOCK_SRC_DRMOCK_TEST_TAGS_H */
