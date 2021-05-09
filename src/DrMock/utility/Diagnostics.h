/* Copyright 2019 Ole Kliemann, Malte Kliemann
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

#ifndef DRMOCK_SRC_UTILITY_DIAGNOSTICS_H
#define DRMOCK_SRC_UTILITY_DIAGNOSTICS_H

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "TypeInfo.h"
#include "TypeTraits.h"

namespace drutility { namespace detail {

template<typename T, typename Enabled = void>
class StreamIfStreamable;

template<typename T>
class StreamIfStreamable<T, typename std::enable_if<is_output_streamable<T>::value>::type>
{
public:
  StreamIfStreamable(const T& t) : t_{t} {}

  std::ostream& operator<<(std::ostream& os) const
  {
    return os << t_;
  }

private:
  const T& t_;
};

template<typename T>
class StreamIfStreamable<T, typename std::enable_if<not is_output_streamable<T>::value>::type>
{
public:
  StreamIfStreamable(const T&) {}

  std::ostream& operator<<(std::ostream& os) const
  {
    return os << "*not printable*";
  }
};

template<typename T>
std::ostream& operator<<(std::ostream& os, const StreamIfStreamable<T>& t)
{
  return t.operator<<(os);
}

template<typename... Ts>
struct PrintAll
{
  void operator()(std::vector<std::string>& strings)
  {}
};

template<typename T>
struct PrintAll<T>
{
  void operator()(std::vector<std::string>& strings, const T& t) const
  {
    std::stringstream s{};
    s << TypeInfo<T>::name() << " = " << StreamIfStreamable<T>{t};
    strings.push_back(s.str());
  }
};

template<typename T, typename... Ts>
struct PrintAll<T, Ts...>
{
  void operator()(std::vector<std::string>& strings, const T& t, const Ts&... ts) const
  {
    PrintAll<T>{}(strings, t);
    PrintAll<Ts...>{}(strings, ts...);
  }
};

}} // namespace drutility::detail

#endif /* DRMOCK_SRC_UTILITY_DIAGNOSTICS_H */
