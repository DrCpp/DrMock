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

#include <DrMock/mock/StateObject.h>

namespace drmock {

class IMethod;

class Controller
{
public:
  Controller(std::vector<std::shared_ptr<IMethod>>);
  Controller(std::vector<std::shared_ptr<IMethod>>, std::shared_ptr<StateObject>);
  bool verify() const;
  bool verifyState(const std::string& state) const;
  bool verifyState(const std::string& state, const std::string& slot) const;
  std::string makeFormattedErrorString() const;

private:
  std::vector<std::shared_ptr<IMethod>> methods_{};
  std::shared_ptr<StateObject> state_object_{};
};

} // namespace drmock

#endif /* DRMOCK_SRC_DRMOCK_MOCK_CONTROLLER_H */
