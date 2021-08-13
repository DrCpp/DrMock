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

#ifndef DRMOCK_TESTS_INTEGRATION_IQUALIFIEROVERLOAD_H
#define DRMOCK_TESTS_INTEGRATION_IQUALIFIEROVERLOAD_H

#include <string>
#include <vector>

namespace outer { namespace inner {

class IQualifierOverload
{
public:
  virtual ~IQualifierOverload() = default;

  virtual void funcOverloadConst() = 0;
  virtual void funcOverloadConst() const = 0;
  virtual std::vector<std::string>& funcOverloadConstDifferentReturn() = 0;
  virtual const std::vector<std::string>& funcOverloadConstDifferentReturn() const = 0;

  virtual std::vector<std::string>& funcOverloadMultipleConstReturns() = 0;
  virtual const std::vector<std::string>& funcOverloadMultipleConstReturns() const = 0;
  virtual std::string& funcOverloadMultipleConstReturns(int) = 0;
  virtual const std::string& funcOverloadMultipleConstReturns(int) const = 0;
  virtual const std::string& funcOverloadMultipleConstReturns(int, float) const = 0;

  virtual void onlyConst(int) const {};
  virtual void onlyConst(float) const {};
  virtual void onlyRef(int) & {};
  virtual void onlyRef(float) & {};
  virtual void mixed(int) const& {};
  virtual void mixed(int) const&& {};
};

}} // namespace outer::inner

#endif /* DRMOCK_TESTS_INTEGRATION_IQUALIFIEROVERLOAD_H */
