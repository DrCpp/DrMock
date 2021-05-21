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
#include <tuple>
#include <unordered_map>
#include <vector>

#include "Tags.h"

namespace drtest { namespace detail {

class TestObject
{
public:
  TestObject() = default;
  explicit TestObject(std::string);

  void setTestFunc(std::function<void()>);
  void setDataFunc(std::function<void()>);
  template<typename T> void addColumn(std::string);
  template<typename... Ts> void addRow(const std::string& row, Ts&&... ts);
  template<typename T> T fetchData(const std::string& column) const;
  void prepareTestData();
  void runTest(bool verbose_logging = true);
  std::size_t num_failures() const;
  void xfail();

private:
  void runOneTest(const std::string& row, bool verbose_logging);

  // Add the elements of the tuple `t` specified by `Is...` to `row`.
  // Shall only be called from `addRow`.
  template<typename Tuple, std::size_t... Is> void addRowImpl(
      const std::string& row,
      Tuple t,
      const std::index_sequence<Is...>&
    );
  // Add `t` to the entry (`row`, `col`) of the data matrix. Shall only
  // be called from `addRow`.
  template<typename T> void addRowImpl(const std::string& row, std::size_t col, T&& t);

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
  std::unordered_map<std::string, tags::Tag> tags_{};  // row -> tags
  std::string current_row_{};
  std::function<void()> data_func_{};
  std::function<void()> test_func_{};
  std::vector<std::string> failed_rows_{};
  bool xfail_{false};
};

}} // namespaces

#include "TestObject.tpp"

#endif /* DRMOCK_SRC_TEST_TESTOBJECT_H */
