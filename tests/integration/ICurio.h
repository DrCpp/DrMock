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

#ifndef DRMOCK_TESTS_INTEGRATION_ICURIO_H
#define DRMOCK_TESTS_INTEGRATION_ICURIO_H

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>

// *** The following will be ignored:

template<typename T, typename U>
using OuterTypeAliasTemplate = std::tuple<T, U>;

using OuterTypeAlias = std::string;

// ***

namespace outer { namespace inner {

class ICurio
{
public:
  using InnerTypeAlias = std::vector<int>;

  template<typename V, typename... Vs>
  using InnerTypeAliasTemplate = std::unordered_map<V, std::tuple<Vs...>>;

  virtual ~ICurio() = default;

  virtual InnerTypeAlias funcUsingParameter(OuterTypeAlias) = 0;
  virtual void funcOddParameters(const int* const) = 0;
  virtual void funcOddParameters(const int* const* const &&) = 0;
  virtual void funcNonCopyableArg(std::unique_ptr<int>) = 0;
  virtual std::unique_ptr<int> funcNonCopyableResult() = 0;
  virtual std::unique_ptr<int>& funcNonCopyableResultAsReference() = 0;
};

}} // namespace outer::inner

#endif /* DRMOCK_TESTS_INTEGRATION_ICURIO_H */
