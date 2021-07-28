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

#ifndef DRMOCK_SRC_DRMOCK_MOCK_EQUAL_H
#define DRMOCK_SRC_DRMOCK_MOCK_EQUAL_H

#include <DrMock/mock/IMatcher.h>
#include <DrMock/mock/detail/IsEqual.h>

namespace drmock {

/**
 * For matching elements using equality.
 *
 *
 */
template<typename Base, typename Derived = Base>
class Equal : public IMatcher<Base>
{
public:
  Equal(const Base& expected)
  :
    expected_{expected}
  {}

  Equal(Base&& expected)
  :
    expected_{std::move(expected)}
  {}

  bool
  match(const Base& actual) const override
  {
    auto is_equal = detail::IsEqual<Base, Derived>{};
    return is_equal(expected_, actual);
  }

private:
  Base expected_;
};

template<typename Base, typename Derived = Base>
std::shared_ptr<Equal<Base, Derived>>
equal(Base expected)
{
  return std::make_shared<Equal<Base, Derived>>(std::move(expected));
}

} // namespace drmock

#endif /* DRMOCK_SRC_DRMOCK_MOCK_EQUAL_H */
