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

#include "Global.h"

namespace drtest {

template<typename T>
void
addColumn(std::string column)
{
  drutility::Singleton<detail::Global>::get()->addColumn<T>(std::move(column));
}

template<typename... Ts>
void
addRow(const std::string& row, Ts&&... ts)
{
  drutility::Singleton<detail::Global>::get()->addRow({}, row, std::forward<Ts>(ts)...);
}

template<typename... Ts>
void
addRow(Mark mark, const std::string& row, Ts&&... ts)
{
  drutility::Singleton<detail::Global>::get()->addRow(mark, row, std::forward<Ts>(ts)...);
}

} // namespace drtest
