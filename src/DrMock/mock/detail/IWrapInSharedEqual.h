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

#ifndef DRMOCK_SRC_DRMOCK_MOCK_DETAIIL_IWRAPINSHAREDEQUAL_H
#define DRMOCK_SRC_DRMOCK_MOCK_DETAIIL_IWRAPINSHAREDEQUAL_H

#include <memory>
#include <tuple>
#include <variant>

#include <DrMock/mock/ICompare.h>
#include <DrMock/mock/detail/Variant.h>

namespace drmock { namespace detail {

template<typename T>
using expect_t = Variant<T, std::shared_ptr<ICompare<T>>>;

template<typename... Bases>
class IWrapInSharedEqual
{
public:
  virtual ~IWrapInSharedEqual() = default;
  virtual std::tuple<std::shared_ptr<ICompare<Bases>>...>
  wrap(expect_t<Bases>&&... pack) = 0;
};

}} // namespace drmock::detail

#endif /* DRMOCK_SRC_DRMOCK_MOCK_DETAIIL_IWRAPINSHAREDEQUAL_H */
