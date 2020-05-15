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

#ifndef SRC_GENERATOR_EXAMPLEMOCK_H
#define SRC_GENERATOR_EXAMPLEMOCK_H

#include "mock/Util.h"
#include "mock/Method.h"
#include "mock/MethodCollection.h"
#include "mock/Qualifiers.h"
#include "IExample.h"


#define DRTEST_MO_METHOD(Name) \
public: \
  template<typename... DRTEST_Ts> auto& Name() \
  { \
    return Name##_dispatch(drmock::TypeContainer<DRTEST_Ts...>{}); \
  }

#define DRTEST_MO_METHOD_OVERLOAD(ResultType, Name, Id) \
private: \
  std::shared_ptr< \
      typename detail::MethodDecayedArgs<ResultType>::type \
    > Id{std::make_shared<typename detail::MethodDecayedArgs<ResultType>::type>()}; \
  auto& Name##_dispatch(drmock::TypeContainer<>) { return *Id; }

#define DRTEST_MO_METHOD_NO_OVERLOAD(...) DRTEST_MO_METHOD_OVERLOAD(__VA_ARGS__)

#define DRTEST_MO_METHOD_OVERLOAD_CONST(ResultType, Name, Id) \
private: \
  std::shared_ptr< \
      typename detail::MethodDecayedArgs<ResultType>::type \
    > Id{std::make_shared<typename detail::MethodDecayedArgs<ResultType>::type>()}; \
  auto& Name##_dispatch(drmock::TypeContainer<drmock::Const>) { return *Id; }

#define DRTEST_MO_METHOD_OVERLOAD_ARGS(ResultType, Name, Id, ...) \
private: \
  std::shared_ptr< \
      typename detail::MethodDecayedArgs<ResultType, __VA_ARGS__>::type \
    > Id{std::make_shared<typename detail::MethodDecayedArgs<ResultType, __VA_ARGS__>::type>()}; \
  auto& Name##_dispatch(drmock::TypeContainer<__VA_ARGS__>) { return *Id; } \

#define DRTEST_MO_METHOD_OVERLOAD_ARGS_CONST(ResultType, Name, Id, ...) \
private: \
  std::shared_ptr< \
      typename detail::MethodDecayedArgs<ResultType, __VA_ARGS__>::type \
    > Id{std::make_shared<typename detail::MethodDecayedArgs<ResultType, __VA_ARGS__>::type>()}; \
  auto& Name##_dispatch(drmock::TypeContainer<__VA_ARGS__, drmock::Const>) { return *Id; } \

#define DRTEST_MO_METHOD_NO_OVERLOAD_ARGS(ResultType, Name, Id, ...) \
private: \
  std::shared_ptr< \
      typename detail::MethodDecayedArgs<ResultType, __VA_ARGS__>::type \
    > Id{std::make_shared<typename detail::MethodDecayedArgs<ResultType, __VA_ARGS__>::type>()}; \
  auto& Name##_dispatch(drmock::TypeContainer<>) { return *Id; } \

#define DRTEST_MO_METHOD_RETURN(ResultType, Statement) \
  auto& result = Statement; \
  if constexpr (std::is_copy_constructible_v<ResultType>) \
  { \
    return result; \
  } \
  else \
  { \
    return std::move(result); \
  }

namespace drmock { namespace mock_implementations {

template<typename T, typename... Ts>
class ExampleMockObject
{
public:
  DRTEST_MO_METHOD(funcVoid0)  //
  DRTEST_MO_METHOD_NO_OVERLOAD(void, funcVoid0, funcVoid0_Method)

  DRTEST_MO_METHOD(funcVoid0Const)
  DRTEST_MO_METHOD_NO_OVERLOAD(void, funcVoid0Const, funcVoid0Const_Method)

  DRTEST_MO_METHOD(funcVoid0Noexcept)
  DRTEST_MO_METHOD_NO_OVERLOAD(void, funcVoid0Noexcept, funcVoid0Noexcept_Method)

  DRTEST_MO_METHOD(funcVoid0ConstNoexcept)
  DRTEST_MO_METHOD_NO_OVERLOAD(void, funcVoid0ConstNoexcept, funcVoid0ConstNoexcept_Method)

  DRTEST_MO_METHOD(funcNonVoid0)
  DRTEST_MO_METHOD_NO_OVERLOAD(int, funcNonVoid0, funcNonVoid0_Method)

  DRTEST_MO_METHOD(funcOverloadConst)
  DRTEST_MO_METHOD_OVERLOAD(void, funcOverloadConst, funcOverloadConst_Method_1)
  DRTEST_MO_METHOD_OVERLOAD_CONST(
      void,
      funcOverloadConst,
      funcOverloadConst_Method_2
    )

  DRTEST_MO_METHOD(funcOverloadConstDifferentReturn)
  DRTEST_MO_METHOD_OVERLOAD(
      std::vector<std::string>&,
      funcOverloadConstDifferentReturn,
      funcOverloadConstDifferentReturn_Method_1
    )
  DRTEST_MO_METHOD_OVERLOAD_CONST(
      const std::vector<std::string>&,
      funcOverloadConstDifferentReturn,
      funcOverloadConstDifferentReturn_Method_2)
    ;

  DRTEST_MO_METHOD(funcOverloadParameters)
  DRTEST_MO_METHOD_OVERLOAD(void, funcOverloadParameters, funcOverloadParameters_Method_1)
  DRTEST_MO_METHOD_OVERLOAD_ARGS(
      void,
      funcOverloadParameters,
      funcOverloadParameters_Method_2,
      std::unordered_map<int, std::string>,
      const float
    )
  DRTEST_MO_METHOD_OVERLOAD_ARGS(
      void,
      funcOverloadParameters,
      funcOverloadParameters_Method_3,
      const std::shared_ptr<std::unordered_map<int, std::string>>&
    )

  DRTEST_MO_METHOD(funcTemplateParameters)
  DRTEST_MO_METHOD_OVERLOAD_ARGS(
      void,
      funcTemplateParameters,
      funcTemplateParameters_Method_1,
      T,
      T
    )
  DRTEST_MO_METHOD_OVERLOAD_ARGS(
      void,
      funcTemplateParameters,
      funcTemplateParameters_Method_2,
      Ts&&...
    )
  DRTEST_MO_METHOD_OVERLOAD_ARGS(
      bool,
      funcTemplateParameters,
      funcTemplateParameters_Method_3,
      const T&,
      Ts&&...
    )
  DRTEST_MO_METHOD_OVERLOAD_ARGS(
      void,
      funcTemplateParameters,
      funcTemplateParameters_Method_4,
      T&&,
      const int&,
      Ts&&...
    )

  DRTEST_MO_METHOD(funcUsingParameter)
  using funcUsingParameter_ResultType = typename outer::inner::IExample<T, Ts...>::InnerTypeAlias;
  DRTEST_MO_METHOD_NO_OVERLOAD_ARGS(
      funcUsingParameter_ResultType,
      funcUsingParameter,
      funcUsingParameter_Method,
      OuterTypeAlias
    )

  DRTEST_MO_METHOD(operatorGreater)
  DRTEST_MO_METHOD_NO_OVERLOAD_ARGS(void, operatorGreater, operatorGreater_Method, int)

  DRTEST_MO_METHOD(operatorEqual)
  DRTEST_MO_METHOD_NO_OVERLOAD_ARGS(void, operatorEqual, operatorEqual_Method, const T&)

  DRTEST_MO_METHOD(operatorPlus)
  DRTEST_MO_METHOD_NO_OVERLOAD_ARGS(void, operatorPlus, operatorPlus_Method, const T&)

  DRTEST_MO_METHOD(funcOverloadLvalueRvalueAndConst)
  DRTEST_MO_METHOD_OVERLOAD_ARGS(
      void,
      funcOverloadLvalueRvalueAndConst,
      funcOverloadLvalueRvalueAndConst_Method_1,
      const std::string&
    )
  DRTEST_MO_METHOD_OVERLOAD_ARGS(
      void,
      funcOverloadLvalueRvalueAndConst,
      funcOverloadLvalueRvalueAndConst_Method_2,
      std::string&
    )
  DRTEST_MO_METHOD_OVERLOAD_ARGS(
      void,
      funcOverloadLvalueRvalueAndConst,
      funcOverloadLvalueRvalueAndConst_Method_3,
      const std::string&&
    )
  DRTEST_MO_METHOD_OVERLOAD_ARGS(
      void,
      funcOverloadLvalueRvalueAndConst,
      funcOverloadLvalueRvalueAndConst_Method_4,
      std::string&&
    )

  DRTEST_MO_METHOD(funcOddParameters)
  DRTEST_MO_METHOD_OVERLOAD_ARGS(
      void,
      funcOddParameters,
      funcOddParameters_Method_1,
      const T* const
    )
  DRTEST_MO_METHOD_OVERLOAD_ARGS(
      void,
      funcOddParameters,
      funcOddParameters_Method_2,
      const T* const* const&&
    )

  DRTEST_MO_METHOD(funcNonCopyableArg)
  DRTEST_MO_METHOD_NO_OVERLOAD_ARGS(
      void,
      funcNonCopyableArg,
      funcNonCopyableArg_Method,
      std::unique_ptr<int>
    )

  DRTEST_MO_METHOD(funcNonCopyableResult)
  DRTEST_MO_METHOD_NO_OVERLOAD(
      std::unique_ptr<int>,
      funcNonCopyableResult,
      funcNonCopyableResult_Method
    )

  DRTEST_MO_METHOD(funcNonCopyableResultAsReference)
  DRTEST_MO_METHOD_NO_OVERLOAD(
      std::unique_ptr<int>&,
      funcNonCopyableResultAsReference,
      funcNonCopyableResultAsReference_Method
    )

private:
  MethodCollection methods{std::vector<std::shared_ptr<IMethod>>{
      funcVoid0_Method,
      funcVoid0Const_Method,
      funcVoid0Noexcept_Method,
      funcVoid0ConstNoexcept_Method,
      funcNonVoid0_Method,
      funcOverloadConst_Method_1,
      funcOverloadConst_Method_2,
      funcOverloadConstDifferentReturn_Method_1,
      funcOverloadConstDifferentReturn_Method_2,
      funcOverloadParameters_Method_1,
      funcOverloadParameters_Method_2,
      funcOverloadParameters_Method_3,
      funcTemplateParameters_Method_1,
      funcTemplateParameters_Method_2,
      funcTemplateParameters_Method_3,
      funcTemplateParameters_Method_4,
      funcUsingParameter_Method,
      operatorGreater_Method,
      operatorEqual_Method,
      operatorPlus_Method,
      funcOverloadLvalueRvalueAndConst_Method_1,
      funcOverloadLvalueRvalueAndConst_Method_2,
      funcOverloadLvalueRvalueAndConst_Method_3,
      funcOverloadLvalueRvalueAndConst_Method_4,
      funcOddParameters_Method_1,
      funcOddParameters_Method_2,
      funcNonCopyableArg_Method,
      funcNonCopyableResult_Method,
      funcNonCopyableResultAsReference_Method
    }};

public:
  bool verify() { return methods.verify(); }
};

}}}

namespace outer { namespace inner {

template<typename T, typename... Ts>
class ExampleMock : public IExample<T, Ts...>
{
public:
  mutable drmock::mock_implementations::ExampleMockObject<T, Ts...> mock{};

  virtual void funcVoid0()
  {
    mock.funcVoid0().call();
  }

  virtual void funcVoid0Const() const
  {
    mock.funcVoid0Const().call();
  }

  virtual void funcVoid0Noexcept() noexcept
  {
    mock.funcVoid0Noexcept().call();
  }

  virtual void funcVoid0ConstNoexcept() const noexcept
  {
    mock.funcVoid0ConstNoexcept().call();
  }

  virtual int funcNonVoid0()
  {
    DRTEST_MO_METHOD_RETURN(int, *mock.funcNonVoid0().call());
  }

  virtual void funcOverloadConst()
  {
    mock.template funcOverloadConst().call();
  }

  virtual void funcOverloadConst() const
  {
    mock.template funcOverloadConst<drmock::Const>().call();
  }

  virtual std::vector<std::string>& funcOverloadConstDifferentReturn()
  {
    auto& result = *mock.template funcOverloadConstDifferentReturn().call();
    if constexpr (std::is_copy_constructible_v<std::vector<std::string> &>)
    {
      return result;
    }
    else
    {
      return std::move(result);
    }
  }

  virtual const std::vector<std::string>& funcOverloadConstDifferentReturn() const
  {
     DRTEST_MO_METHOD_RETURN(
         const std::vector<std::string>&,
         *mock.template funcOverloadConstDifferentReturn<drmock::Const>().call()
       );
  }

  virtual void funcOverloadParameters()
  {
    mock.template funcOverloadParameters<>().call();
  }

  virtual void funcOverloadParameters(
      std::unordered_map<int, std::string> a1,
      const float a2
    )
  {
    mock.template funcOverloadParameters<
        std::unordered_map<int, std::string>,
        const float
      >().call(std::move(a1), std::move(a2));
  }

  virtual void funcOverloadParameters(
      const std::shared_ptr<std::unordered_map<int, std::string>>& a1
    )
  {
     mock.template funcOverloadParameters<
        const std::shared_ptr<std::unordered_map<int, std::string>>&
      >().call(a1); // no move, it's a reference
  }

  virtual void funcTemplateParameters(T a1, T a2)
  {
    mock.template funcTemplateParameters<T, T>().call(std::move(a1), std::move(a2));
  }

  virtual void funcTemplateParameters(Ts&&... a1)
  {
    mock.template funcTemplateParameters<Ts&&...>().call(std::move(a1)...);
  }

  virtual bool funcTemplateParameters(const T& a1, Ts&&... a2)
  {
    DRTEST_MO_METHOD_RETURN(
        bool,
        (*mock.template funcTemplateParameters<const T&, Ts&&...>().call(a1, std::move(a2)...))
        // note the parentheses around the expression to escape the comma used
      );
  }

  virtual void funcTemplateParameters(T&& a1, const int& a2, Ts&&... a3)
  {
    mock.template funcTemplateParameters<
        T&&,
        const int&,
        Ts&&...
      >().call(std::move(a1), a2, std::move(a3)...);
  }

  virtual typename IExample<T, Ts...>::InnerTypeAlias funcUsingParameter(OuterTypeAlias a1)
  {
    using ResultType = typename IExample<T, Ts...>::InnerTypeAlias;
    DRTEST_MO_METHOD_RETURN(ResultType, *mock.funcUsingParameter().call(std::move(a1)));
  }

  virtual void operator>(int a1)
  {
    mock.operatorGreater().call(std::move(a1));
  }

  virtual void operator==(const T& a1)
  {
    mock.operatorEqual().call(a1);
  }

  virtual void operator+(const T& a1)
  {
    mock.operatorPlus().call(a1);
  }

  virtual void funcOverloadLvalueRvalueAndConst(const std::string& a1)
  {
    mock.template funcOverloadLvalueRvalueAndConst<const std::string&>().call(a1);
  }

  virtual void funcOverloadLvalueRvalueAndConst(std::string& a1)
  {
    mock.template funcOverloadLvalueRvalueAndConst<std::string&>().call(a1);
  }

  virtual void funcOverloadLvalueRvalueAndConst(const std::string&& a1)
  {
    mock.template funcOverloadLvalueRvalueAndConst<const std::string&&>().call(a1);
  }

  virtual void funcOverloadLvalueRvalueAndConst(std::string&& a1)
  {
    mock.template funcOverloadLvalueRvalueAndConst<std::string&&>().call(std::move(a1));
  }

  virtual void funcOddParameters(const T* const a1)
  {
    mock.template funcOddParameters<const T* const>().call(std::move(a1));
  }

  virtual void funcOddParameters(const T* const* const && a1)
  {
    mock.template funcOddParameters<const T* const* const&&>().call(std::move(a1));
  }

  virtual void funcNonCopyableArg(std::unique_ptr<int> a1)
  {
    mock.funcNonCopyableArg().call(std::move(a1));
  }

  virtual std::unique_ptr<int> funcNonCopyableResult()
  {
    DRTEST_MO_METHOD_RETURN(std::unique_ptr<int>, *mock.funcNonCopyableResult().call());
  }

  virtual std::unique_ptr<int>& funcNonCopyableResultAsReference()
  {
    DRTEST_MO_METHOD_RETURN(
        std::unique_ptr<int>&,
        *mock.funcNonCopyableResultAsReference().call()
      );
  }
};

}} // namespace

#endif /* SRC_GENERATOR_EXAMPLEMOCK_H */
