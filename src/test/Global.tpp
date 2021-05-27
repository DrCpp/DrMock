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

namespace drtest { namespace detail {

template<typename T>
void
Global::addColumn(std::string column)
{
  tests_[current_test_].addColumn<T>(std::move(column));
}

template<typename... Ts>
void
Global::addRow(const std::string& row, Ts&&... ts)
{
  tests_[current_test_].addRow(row, 0, std::forward<Ts>(ts)...);
}

template<typename T>
T
Global::fetchData(const std::string& column)
{
  return tests_[current_test_].fetchData<T>(column);
}

template<typename T>
bool
Global::almostEqual(T actual, T expected)
{
  return tests_[current_test_].almostEqual(actual, expected);
}

}} // namespaces
