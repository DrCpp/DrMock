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

#ifndef DRMOCK_SRC_DRMOCK_MOCK_CONTROLLER_H
#define DRMOCK_SRC_DRMOCK_MOCK_CONTROLLER_H

#include <memory>
#include <vector>

namespace drmock {

class IMethod;
class StateObject;

/**
 * Class for sharing one state object among a collection of `Method`
 * objects.
 */
class Controller
{
public:
  /**
   * @param methods The collection of methods
   */
  Controller(std::vector<std::shared_ptr<IMethod>>);

  /**
   * @param methods The collection of methods
   * @param state_object The shared state object
   */
  Controller(
      std::vector<std::shared_ptr<IMethod>> methods,
      std::shared_ptr<StateObject> state_object
    );

  /**
   * Verify all methods in the collection.
   */
  bool verify() const;

  /**
   * Check if the default slot has current state `state`.
   */
  bool verifyState(const std::string& state) const;
  /**
   * Check if `slot` has current state `state`.
   */
  bool verifyState(const std::string& slot, const std::string& state) const;

  /**
   * Return the concatenation of the error strings of the method
   * collection.
   */
  std::string makeFormattedErrorString() const;

private:
  std::vector<std::shared_ptr<IMethod>> methods_{};  /**> The method collection */
  std::shared_ptr<StateObject> state_object_{};  /**> The shared state object */
};

} // namespace drmock

#endif /* DRMOCK_SRC_DRMOCK_MOCK_CONTROLLER_H */
