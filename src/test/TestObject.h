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

#ifndef DRMOCK_SRC_TEST_TESTOBJECT_H
#define DRMOCK_SRC_TEST_TESTOBJECT_H

#include <any>
#include <functional>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include "../utility/Compare.h"

namespace drtest { namespace detail {

class TestObject
{
public:
  TestObject() = default;
  explicit TestObject(std::string);

  void setTestFunc(std::function<void()>);
  void setDataFunc(std::function<void()>);
  template<typename T> void addColumn(std::string);
  template<typename T, typename... Ts> void addRow(
      const std::string& row,
      std::size_t i,
      T&& t, Ts&&... ts
    );
  template<typename T> T fetchData(const std::string& column) const;
  void prepareTestData();
  void runTest(bool verbose_logging = true);
  std::size_t num_failures() const;
  template<typename T> bool almostEqual(T actual, T expected) const;
  void abs_tol(double value);
  void rel_tol(double value);

private:
  void runOneTest(const std::string& row, bool verbose_logging);

  std::string name_{};
  std::vector<std::string> data_columns_{};
  std::unordered_map<std::string, std::type_index> data_column_types_{};
  std::vector<std::string> data_rows_{};
  std::unordered_map<
      std::string, // row
      std::unordered_map<
          std::string, // column
          std::any
        >> data_sets_{};
  std::string current_row_{};
  std::function<void()> data_func_{};
  std::function<void()> test_func_{};
  std::vector<std::string> failed_rows_{};

  double abs_tol_ = DRTEST_ABS_TOL;
  double rel_tol_ = DRTEST_REL_TOL;
};

}} // namespaces

#include "TestObject.tpp"

#endif /* DRMOCK_SRC_TEST_TESTOBJECT_H */
