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

#ifndef DRMOCK_TESTS_INTEGRATION_ITEMPLATE_H
#define DRMOCK_TESTS_INTEGRATION_ITEMPLATE_H

#include <tuple>

namespace outer { namespace inner {

template<typename T, typename... Ts>
class ITemplate
{
public:
  virtual ~ITemplate() = default;

  virtual void fTemplate(const Ts&... x) = 0;
  virtual bool gTemplate(const T&, Ts&&... x) = 0;
  virtual std::tuple<T, T, Ts...> hTemplate(T&&, const int&, Ts* const... x) = 0;
};

}} // namespace outer::inner

#endif /* DRMOCK_TESTS_INTEGRATION_ITEMPLATE_H */
