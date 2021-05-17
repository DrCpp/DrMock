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

#include <iostream>
#include <stdexcept>

namespace drtest { namespace detail {

template<typename T>
void
TestObject::addColumn(std::string column)
{
  data_columns_.push_back(column);
  data_column_types_.insert({
      std::move(column),
      std::type_index(typeid(typename std::decay<T>::type))
    });
}

template<typename T, typename... Ts>
void
TestObject::addRow(
    Mark mark,
    const std::string& row,
    std::size_t i,
    T&& t, Ts&&... ts
  )
{
  addRow(row, i, std::forward<T>(t), std::forward<Ts>(ts)...);
  marks_[row] = mark;
}

template<typename T, typename... Ts>
void
TestObject::addRow(
    const std::string& row,
    std::size_t i,
    T&& t, Ts&&... ts
  )
{
  if ((sizeof...(ts) + 1) > data_columns_.size())
  {
    throw std::logic_error{
        "adding row: \"" + row + "\"" +
        ": with " + std::to_string((sizeof...(ts) + 1)) + " columns, " +
        "can only have " + std::to_string(data_columns_.size())
      };
  }

  if (i == 0)
  {
    data_rows_.push_back(row);
  }

  const std::string& column = data_columns_[i];
  auto type_it = data_column_types_.find(column);
  if (type_it == data_column_types_.end())
  {
    throw std::logic_error{"internal error: no such column"};
  }

  const std::type_index& expected_type = std::get<std::type_index>(*type_it);
  const std::type_index actual_type = std::type_index(typeid(typename std::decay<T>::type));
  if (actual_type != expected_type)
  {
    throw std::logic_error{
        "adding row: \"" + row + "\", " +
        "column: \"" + column + "\": " +
        "type mismatch:\n" +
        "  (actual) " + actual_type.name() + "\n" +
        "  (expected) " + expected_type.name()
      };
  }

  data_sets_[row].insert({column, std::make_any<T>(std::forward<T>(t))});

  if constexpr(sizeof...(ts) != 0)
  {
    addRow(row, i + 1, std::forward<Ts>(ts)...);
  }
}

template<typename T>
T
TestObject::fetchData(const std::string& column) const
{
  if (current_row_.empty())
  {
    throw std::logic_error{"no data provided for test: " + name_};
  }

  auto row_it = data_sets_.find(current_row_);
  if (row_it == data_sets_.end())
  {
    throw std::logic_error{"internal error: no such row: " + current_row_};
  }
  auto& row = std::get<1>(*row_it);

  auto value_it = row.find(column);
  if (value_it == row.end())
  {
    throw std::logic_error{"no such column: " + column};
  }

  return std::any_cast<T>(std::get<std::any>(*value_it));
}

}} // namespaces
