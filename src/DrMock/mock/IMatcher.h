/* Copyright 2021 Ole Kliemann, Malte Kliemann
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

#ifndef DRMOCK_SRC_DRMOCK_MOCK_IMATCHER_H
#define DRMOCK_SRC_DRMOCK_MOCK_IMATCHER_H

namespace drmock {

/**
 * Matches objects of type `Base` against some pattern.
 *
 * See `Equal` (matches by equality) and `AlmostEqual` (approximate
 * equality of floating-point numbers)for sample implementations.
 */
template<typename Base>
class IMatcher
{
public:
  virtual ~IMatcher() = default;
  virtual bool match(const Base&) const = 0;
};

} // namespace drmock

#endif /* DRMOCK_SRC_DRMOCK_MOCK_IMATCHER_H */
