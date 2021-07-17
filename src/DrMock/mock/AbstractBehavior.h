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

#ifndef DRMOCK_SRC_DRMOCK_MOCK_ABSTRACTBEHAVIOR_H
#define DRMOCK_SRC_DRMOCK_MOCK_ABSTRACTBEHAVIOR_H

#include <exception>
#include <memory>
#include <utility>
#include <variant>

#include <DrMock/mock/AbstractSignal.h>
#include <DrMock/mock/detail/IMakeTupleOfMatchers.h>

namespace drmock {

// Abstract class template that represents a method's behavior.
template<typename Class, typename Result, typename... Args>
class AbstractBehavior
{
public:
  virtual ~AbstractBehavior() = default;

  // The produced output is decided by matching the input with
  // implementation specific data.
  virtual std::variant<
      std::monostate,
      std::pair<
          std::shared_ptr<typename std::decay<Result>::type>,
          std::shared_ptr<AbstractSignal<Class>>
        >,
      std::exception_ptr
    > call(const Args&...) = 0;
};

} // namespace drmock

#endif /* DRMOCK_SRC_DRMOCK_MOCK_ABSTRACTBEHAVIOR_H */
