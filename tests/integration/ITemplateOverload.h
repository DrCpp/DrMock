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

#ifndef DRMOCK_TESTS_INTEGRATION_ITEMPLATEOVERLOAD_H
#define DRMOCK_TESTS_INTEGRATION_ITEMPLATEOVERLOAD_H

#include <tuple>

namespace outer { namespace inner {

template<typename T, typename... Ts>
class ITemplateOverload
{
public:
  virtual ~ITemplateOverload() = default;

  virtual void funcTemplateParameters(T, T) = 0;
  virtual void funcTemplateParameters(Ts&&...) = 0;
  virtual bool funcTemplateParameters(const T&, Ts&&...) = 0;
  virtual void funcTemplateParameters(T&&, const int&, Ts&&...) = 0;
};

}} // namespace outer::inner

#endif /* DRMOCK_TESTS_INTEGRATION_ITEMPLATEOVERLOAD_H */
