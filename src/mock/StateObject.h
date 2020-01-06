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

#ifndef DRMOCK_SRC_MOCK_STATEOBJECT_H
#define DRMOCK_SRC_MOCK_STATEOBJECT_H

#include <string>
#include <unordered_map>

namespace drmock {

class StateObject
{
public:
  std::string get() const;
  std::string get(const std::string& slot) const;
  void set(std::string state);
  void set(const std::string& slot, std::string state);

private:
  std::string default_slot_{};
  mutable std::unordered_map<std::string, std::string> slots_{};
}; 

} // namespace drmock

#endif /* DRMOCK_SRC_MOCK_STATEOBJECT_H */
