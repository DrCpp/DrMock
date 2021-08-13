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

#ifndef DRMOCK_TESTS_INTEGRATION_IEXAMPLE_H
#define DRMOCK_TESTS_INTEGRATION_IEXAMPLE_H

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

template<typename T, typename... Ts>
class IExample
{
public:
  using InnerTypeAlias = std::vector<T>;

  template<typename V, typename... Vs>
  using InnerTypeAliasTemplate = std::unordered_map<V, std::tuple<Vs...>>;

  virtual ~IExample() = default;

  virtual void funcVoid0() = 0;
  virtual void funcVoid0Const() const = 0;
  virtual void funcVoid0Noexcept() noexcept = 0;
  virtual void funcVoid0ConstNoexcept() const noexcept = 0;
  virtual int funcNonVoid0() = 0;
  virtual void funcOverloadConst() = 0;
  virtual void funcOverloadConst() const = 0;
  virtual std::vector<std::string>& funcOverloadConstDifferentReturn() = 0;
  virtual const std::vector<std::string>& funcOverloadConstDifferentReturn() const = 0;
  virtual void funcOverloadParameters() = 0;
  virtual void funcOverloadParameters(std::unordered_map<int, std::string>, const float) = 0;
  virtual void funcOverloadParameters(const std::shared_ptr<std::unordered_map<int, std::string>>& test_name) = 0;
  virtual void funcTemplateParameters(T, T) = 0;
  virtual void funcTemplateParameters(Ts&&...) = 0;
  virtual bool funcTemplateParameters(const T&, Ts&&...) = 0;
  virtual void funcTemplateParameters(T&&, const int&, Ts&&...) = 0;
  virtual InnerTypeAlias funcUsingParameter(OuterTypeAlias) = 0;
  virtual void operator>(int) = 0;
  virtual void operator==(const T&) = 0;
  virtual void operator+(const T&) = 0;
  virtual void funcOverloadLvalueRvalueAndConst(const std::string&) = 0;
  virtual void funcOverloadLvalueRvalueAndConst(std::string&) = 0;
  virtual void funcOverloadLvalueRvalueAndConst(const std::string&&) = 0;
  virtual void funcOverloadLvalueRvalueAndConst(std::string&&) = 0;
  virtual void funcOddParameters(const T* const) = 0;
  virtual void funcOddParameters(const T* const* const &&) = 0;
  virtual void funcNonCopyableArg(std::unique_ptr<int>) = 0;
  virtual std::unique_ptr<int> funcNonCopyableResult() = 0;
  virtual std::unique_ptr<int>& funcNonCopyableResultAsReference() = 0;
};

}} // namespace outer::inner

#endif /* DRMOCK_TESTS_INTEGRATION_IEXAMPLE_H */
