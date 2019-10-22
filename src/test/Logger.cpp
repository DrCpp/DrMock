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

#include "Logger.h"

#include <iostream>

namespace drtest { namespace detail {

Logger::Logger()
:
  out_stream_{std::cout.rdbuf()}
{}

void
Logger::logMessage(
    bool timestamp,
    const std::string& category,
    const std::string& location,
    int line,
    const std::ostream& msg
  )
{
  std::lock_guard lck{mtx_};
  bool written = false;
  if (timestamp)
  {
    out_stream_ << mkTimestamp();
    written = true;
  }
  if (not category.empty())
  {
    out_stream_ << category.substr(0, 6)
                << std::string(7 - std::min(category.size(), 6ul), ' ');
    written = true;
  }
  if (not location.empty())
  {
    out_stream_ << location;
    written = true;
  }
  if (line > 0)
  {
    out_stream_ << " (" << line << ")";
    written = true;
  }
  if (msg.rdbuf()->in_avail())
  {
    if (written)
    {
      out_stream_ << ": ";
    }
    out_stream_ << msg.rdbuf();
  }
  out_stream_ << std::endl;
}

std::string
Logger::mkTimestamp()
{
  return {};
}

}} // namespace drtest::detail
