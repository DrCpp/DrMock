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

#ifndef DRMOCK_SRC_DRMOCK_MOCK_STATEOBJECT_H
#define DRMOCK_SRC_DRMOCK_MOCK_STATEOBJECT_H

#include <string>
#include <unordered_map>

namespace drmock {

// Stores _slots_ (by name) and their _state_. Slots that are stored in
// the StateObject are called _registered_. Upon construction, only the
// default slot `""` (empty string) is registered with state `""`.
class StateObject
{
public:
  // Get the state of `slot` (resp. the default slot).
  //
  // If `slot` is not registered yet, it is registered with state `""`
  // before returning `""`.
  std::string get(const std::string& slot);
  std::string get();

  // Set the state of `slot` (resp. the default slot) to `state`.
  void set(const std::string& slot, std::string state);
  void set(std::string state);

private:
  /* map: slot -> state */
  std::unordered_map<std::string, std::string> slots_{};
};

} // namespace drmock

#endif /* DRMOCK_SRC_DRMOCK_MOCK_STATEOBJECT_H */
