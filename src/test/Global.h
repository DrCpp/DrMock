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

#ifndef DRMOCK_SRC_TEST_GLOBAL_H
#define DRMOCK_SRC_TEST_GLOBAL_H

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "TestObject.h"
#include "../utility/Singleton.h"

namespace drtest { namespace detail {

class Global
{
public:
  Global();

  void addTestFunc(const std::string&, std::function<void()>);
  void addDataFunc(const std::string&, std::function<void()>);
  template<typename T> void addColumn(std::string);
  template<typename... Ts> void addRow(const std::string& row, Ts&&... ts);
  template<typename T> T fetchData(const std::string& column);
  void runTestsAndLog();
  std::size_t num_failures() const;

  void mark(Mark);

private:
  void addTest(std::string);
  void runTests();

  std::unordered_set<std::string> reserved_names_;
  std::vector<std::string> test_names_;
  std::unordered_map<
      std::string, // test name
      TestObject
    > tests_;

  std::string current_test_;
};

}} // namespaces

#include "Global.tpp"

#endif /* DRMOCK_SRC_TEST_GLOBAL_H */
