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

#include "../utility/Tuples.h"

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

template<typename... Ts>
void
TestObject::addRow(const std::string& row, Ts&&... ts)
{
  constexpr auto size = sizeof...(ts);
  if constexpr(std::is_same_v<drutility::last_t<Ts...>, tags::Tag>)
  {
    assert(size == (data_columns_.size() + 1));
    tags_[row] = std::get<size-1>(std::forward_as_tuple(ts...));
    addRowImpl(
        row,
        std::forward_as_tuple(ts...),
        std::make_index_sequence<size - 1>{}
      );
  }
  else
  {
    tags_[row] = tags::none;
    addRowImpl(
        row,
        std::forward_as_tuple(ts...),
        std::make_index_sequence<size>{}
      );
  }
}

template<typename Tuple, std::size_t... Is>
void
TestObject::addRowImpl(const std::string& row, Tuple t, const std::index_sequence<Is...>&)
{
  constexpr auto size = sizeof...(Is);
  if (size > data_columns_.size())
  {
    throw std::logic_error{
        "adding row: \"" + row + "\"" +
        ": with " + std::to_string(size) + " columns, " +
        "can only have " + std::to_string(data_columns_.size())
      };
  }
  data_rows_.push_back(row);
  (addRowImpl(row, Is, std::forward<std::tuple_element_t<Is, Tuple>>(std::get<Is>(t))),
   ...);
}

template<typename T>
void
TestObject::addRowImpl(const std::string& row, std::size_t col, T&& t)
{
  const std::string& column = data_columns_[col];
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
