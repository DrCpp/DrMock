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

#ifndef DRMOCK_SRC_TEST_ILOGGER_H
#define DRMOCK_SRC_TEST_ILOGGER_H

#include <ostream>
#include <sstream>
#include <string>

#include "Singleton.h"

namespace drtest { namespace detail {

class ILogger
{
public:
  virtual ~ILogger() = default;
  virtual void logMessage(
      bool timestamp,
      const std::string& category,
      const std::string& location,
      int line,
      const std::ostream& msg
    ) = 0;
};

}} // namespace drtest::detail

#define DRTEST_LOG(category, msg) \
do { \
  try { \
    drtest::detail::Singleton<drtest::detail::ILogger>::get()->logMessage( \
        true, \
        category, \
        __FUNCTION__, \
        __LINE__, \
        std::stringstream{} << msg \
      ); \
   } \
   catch(...) \
   {} \
} while (false)
#define DRTEST_LOG_DEBUG(msg) DRTEST_LOG("DEBUG", msg)
#define DRTEST_LOG_INFO(msg) DRTEST_LOG("INFO", msg)
#define DRTEST_LOG_WARN(msg) DRTEST_LOG("WARN", msg)
#define DRTEST_LOG_CRIT(msg) DRTEST_LOG("CRIT", msg)

#endif /* DRMOCK_SRC_TEST_ILOGGER_H */
