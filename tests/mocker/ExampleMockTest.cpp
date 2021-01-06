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

#include <drmock/test/Test.h>
#include "mock/ExampleMock.h"

using namespace outer::inner;

DRTEST_TEST(fails)
{
  {
    ExampleMock<int, float, double> mock{};
    DRTEST_ASSERT(mock.mock.verify());
  }

  {
    ExampleMock<int, float, double> mock{};
    DRTEST_ASSERT(mock.mock.funcVoid0().verify());
    mock.funcVoid0();
    DRTEST_ASSERT(not mock.mock.verify());
    DRTEST_ASSERT(not mock.mock.funcVoid0().verify());
  }

  {
    ExampleMock<int, float, double> mock{};
    DRTEST_ASSERT(mock.mock.funcVoid0Const().verify());
    mock.funcVoid0Const();
    DRTEST_ASSERT(not mock.mock.verify());
    DRTEST_ASSERT(not mock.mock.funcVoid0Const().verify());
  }

  {
    ExampleMock<int, float, double> mock{};
    DRTEST_ASSERT(mock.mock.funcVoid0Noexcept().verify());
    mock.funcVoid0Noexcept();
    DRTEST_ASSERT(not mock.mock.verify());
    DRTEST_ASSERT(not mock.mock.funcVoid0Noexcept().verify());
  }

  {
    ExampleMock<int, float, double> mock{};
    DRTEST_ASSERT(mock.mock.funcVoid0ConstNoexcept().verify());
    mock.funcVoid0ConstNoexcept();
    DRTEST_ASSERT(not mock.mock.verify());
    DRTEST_ASSERT(not mock.mock.funcVoid0ConstNoexcept().verify());
  }

  {
    ExampleMock<int, float, double> mock{};
    DRTEST_ASSERT(mock.mock.funcNonVoid0().verify());
    mock.funcNonVoid0();
    DRTEST_ASSERT(not mock.mock.verify());
    DRTEST_ASSERT(not mock.mock.funcNonVoid0().verify());
  }

  {
    ExampleMock<int, float, double> mock{};
    DRTEST_ASSERT(mock.mock.funcOverloadConst<>().verify());
    mock.funcOverloadConst();
    DRTEST_ASSERT(not mock.mock.verify());
    DRTEST_ASSERT(not mock.mock.funcOverloadConst<>().verify());
  }

  {
    ExampleMock<int, float, double> mock{};
    DRTEST_ASSERT(mock.mock.funcOverloadConst<drmock::Const>().verify());
    const_cast<const ExampleMock<int, float, double>&>(mock).funcOverloadConst();
    DRTEST_ASSERT(not mock.mock.verify());
    DRTEST_ASSERT(not mock.mock.funcOverloadConst<drmock::Const>().verify());
  }

  {
    ExampleMock<int, float, double> mock{};
    DRTEST_ASSERT(mock.mock.funcOverloadConstDifferentReturn<>().verify());
    mock.funcOverloadConstDifferentReturn();
    DRTEST_ASSERT(not mock.mock.verify());
    DRTEST_ASSERT(not mock.mock.funcOverloadConstDifferentReturn<>().verify());
  }

  {
    ExampleMock<int, float, double> mock{};
    DRTEST_ASSERT(
        mock.mock.funcOverloadConstDifferentReturn<drmock::Const>().verify()
      );
    const_cast<const ExampleMock<int, float, double>&>(mock).funcOverloadConstDifferentReturn();
    DRTEST_ASSERT(not mock.mock.verify());
    DRTEST_ASSERT(
        not mock.mock.funcOverloadConstDifferentReturn<drmock::Const>().verify()
      );
  }

  {
    ExampleMock<int, float, double> mock{};
    bool verified = mock.mock.funcOverloadParameters().verify();
    DRTEST_ASSERT(verified);
    mock.funcOverloadParameters();
    DRTEST_ASSERT(not mock.mock.verify());
    verified = mock.mock.funcOverloadParameters().verify();
    DRTEST_ASSERT(not verified);
  }

  {
    ExampleMock<int, float, double> mock{};
    bool verified = mock.mock.funcOverloadParameters<
        std::unordered_map<int, std::string>,
        const float
      >().verify();
    DRTEST_ASSERT(verified);
    std::unordered_map<int, std::string> a1{};
    const float a2{};
    mock.funcOverloadParameters(a1, a2);
    DRTEST_ASSERT(not mock.mock.verify());
    verified = mock.mock.funcOverloadParameters<
        std::unordered_map<int, std::string>,
        const float
      >().verify();
    DRTEST_ASSERT(not verified);
  }

  {
    ExampleMock<int, float, double> mock{};
    bool verified = mock.mock.funcOverloadParameters<
        const std::shared_ptr<std::unordered_map<int, std::string>>&
      >().verify();
    DRTEST_ASSERT(verified);
    std::shared_ptr<std::unordered_map<int, std::string>> a1{};
    mock.funcOverloadParameters(a1);
    DRTEST_ASSERT(not mock.mock.verify());
    verified = mock.mock.funcOverloadParameters<
        const std::shared_ptr<std::unordered_map<int, std::string>>&
      >().verify();
    DRTEST_ASSERT(not verified);
  }

  {
    ExampleMock<int, float, double> mock{};
    bool verified = mock.mock.funcTemplateParameters<int, int>().verify();
    DRTEST_ASSERT(verified);
    mock.funcTemplateParameters(int{}, int{});
    DRTEST_ASSERT(not mock.mock.verify());
    verified = mock.mock.funcTemplateParameters<int, int>().verify();
    DRTEST_ASSERT(not verified);
  }

  {
    ExampleMock<int, float, double> mock{};
    bool verified = mock.mock.funcTemplateParameters<float&&, double&&>().verify();
    DRTEST_ASSERT(verified);
    mock.funcTemplateParameters(float{}, double{});
    DRTEST_ASSERT(not mock.mock.verify());
    verified = mock.mock.funcTemplateParameters<float&&, double&&>().verify();
    DRTEST_ASSERT(not verified);
  }

  {
    ExampleMock<int, float, double> mock{};
    bool verified = mock.mock.funcTemplateParameters<const int&, float&&, double&&>().verify();
    DRTEST_ASSERT(verified);
    mock.funcTemplateParameters(int{}, float{}, double{});
    DRTEST_ASSERT(not mock.mock.verify());
    verified = mock.mock.funcTemplateParameters<const int&, float&&, double&&>().verify();
    DRTEST_ASSERT(not verified);
  }

  {
    ExampleMock<int, float, double> mock{};
    bool verified = mock.mock.funcTemplateParameters<
        int&&, const int&, float&&, double&&
      >().verify();
    DRTEST_ASSERT(verified);
    mock.funcTemplateParameters(int{}, int{}, float{}, double{});
    DRTEST_ASSERT(not mock.mock.verify());
    verified = mock.mock.funcTemplateParameters<
        int&&, const int&, float&&, double&&
      >().verify();
    DRTEST_ASSERT(not verified);
  }

  {
    ExampleMock<int, float, double> mock{};
    DRTEST_ASSERT(mock.mock.funcUsingParameter().verify());
    mock.funcUsingParameter({});
    DRTEST_ASSERT(not mock.mock.verify());
    DRTEST_ASSERT(not mock.mock.funcUsingParameter().verify());
  }

  {
    ExampleMock<int, float, double> mock{};
    DRTEST_ASSERT(mock.mock.operatorGreater().verify());
    mock.operator>({});
    DRTEST_ASSERT(not mock.mock.verify());
    DRTEST_ASSERT(not mock.mock.operatorGreater().verify());
  }

  {
    ExampleMock<int, float, double> mock{};
    DRTEST_ASSERT(mock.mock.operatorEqual().verify());
    mock.operator==({});
    DRTEST_ASSERT(not mock.mock.verify());
    DRTEST_ASSERT(not mock.mock.operatorEqual().verify());
  }

  {
    ExampleMock<int, float, double> mock{};
    DRTEST_ASSERT(mock.mock.operatorPlus().verify());
    mock.operator+({});
    DRTEST_ASSERT(not mock.mock.verify());
    DRTEST_ASSERT(not mock.mock.operatorPlus().verify());
  }

  {
    ExampleMock<int, float, double> mock{};
    DRTEST_ASSERT(
        mock.mock.funcOverloadLvalueRvalueAndConst<const std::string&>().verify()
      );
    const std::string a1{};
    mock.funcOverloadLvalueRvalueAndConst(a1);
    DRTEST_ASSERT(not mock.mock.verify());
    DRTEST_ASSERT(
        not mock.mock.funcOverloadLvalueRvalueAndConst<const std::string&>().verify()
      );
  }

  {
    ExampleMock<int, float, double> mock{};
    DRTEST_ASSERT(mock.mock.funcOverloadLvalueRvalueAndConst<std::string&>().verify());
    std::string a1{};
    mock.funcOverloadLvalueRvalueAndConst(a1);
    DRTEST_ASSERT(not mock.mock.verify());
    DRTEST_ASSERT(not mock.mock.funcOverloadLvalueRvalueAndConst<std::string&>().verify());
  }

  {
    ExampleMock<int, float, double> mock{};
    DRTEST_ASSERT(
        mock.mock.funcOverloadLvalueRvalueAndConst<const std::string&&>().verify()
      );
    const std::string a1{};
    mock.funcOverloadLvalueRvalueAndConst(std::move(a1));
    DRTEST_ASSERT(not mock.mock.verify());
    DRTEST_ASSERT(
        not mock.mock.funcOverloadLvalueRvalueAndConst<const std::string&&>().verify()
      );
  }

  {
    ExampleMock<int, float, double> mock{};
    DRTEST_ASSERT(mock.mock.funcOverloadLvalueRvalueAndConst<std::string&&>().verify());
    std::string a1{};
    mock.funcOverloadLvalueRvalueAndConst(std::move(a1));
    DRTEST_ASSERT(not mock.mock.verify());
    DRTEST_ASSERT(not mock.mock.funcOverloadLvalueRvalueAndConst<std::string&&>().verify());
  }

  {
    ExampleMock<int, float, double> mock{};
    DRTEST_ASSERT(mock.mock.funcOddParameters<const int* const>().verify());
    const int* const a1{};
    mock.funcOddParameters(a1);
    DRTEST_ASSERT(not mock.mock.verify());
    DRTEST_ASSERT(not mock.mock.funcOddParameters<const int* const>().verify());
  }

  {
    ExampleMock<int, float, double> mock{};
    DRTEST_ASSERT(mock.mock.funcOddParameters<const int* const* const&&>().verify());
    const int* const* const a1{};
    mock.funcOddParameters(std::move(a1));
    DRTEST_ASSERT(not mock.mock.verify());
    DRTEST_ASSERT(not mock.mock.funcOddParameters<const int* const* const&&>().verify());
  }

  {
    ExampleMock<int, float, double> mock{};
    DRTEST_ASSERT(mock.mock.funcNonCopyableArg().verify());
    mock.funcNonCopyableArg({});
    DRTEST_ASSERT(not mock.mock.verify());
    DRTEST_ASSERT(not mock.mock.funcNonCopyableArg().verify());
  }

  {
    ExampleMock<int, float, double> mock{};
    DRTEST_ASSERT(mock.mock.funcNonCopyableResult().verify());
    mock.funcNonCopyableResult();
    DRTEST_ASSERT(not mock.mock.verify());
    DRTEST_ASSERT(not mock.mock.funcNonCopyableResult().verify());
  }

  {
    ExampleMock<int, float, double> mock{};
    DRTEST_ASSERT(mock.mock.funcNonCopyableResultAsReference().verify());
    mock.funcNonCopyableResultAsReference();
    DRTEST_ASSERT(not mock.mock.verify());
    DRTEST_ASSERT(not mock.mock.funcNonCopyableResultAsReference().verify());
  }
}

DRTEST_TEST(succeeds)
{
  ExampleMock<int, float, double> mock{};
  DRTEST_ASSERT(mock.mock.verify());

  mock.mock.funcVoid0().push()
      .expects()
      .times(1);
  mock.funcVoid0();
  DRTEST_ASSERT(mock.mock.verify());
  DRTEST_ASSERT(mock.mock.funcVoid0().verify());

  mock.mock.funcVoid0Const().push()
      .expects()
      .times(1);
  mock.funcVoid0Const();
  DRTEST_ASSERT(mock.mock.verify());
  DRTEST_ASSERT(mock.mock.funcVoid0Const().verify());

  mock.mock.funcVoid0Noexcept().push()
      .expects()
      .times(1);
  mock.funcVoid0Noexcept();
  DRTEST_ASSERT(mock.mock.verify());
  DRTEST_ASSERT(mock.mock.funcVoid0Noexcept().verify());

  mock.mock.funcVoid0ConstNoexcept().push()
      .expects()
      .times(1);
  mock.funcVoid0ConstNoexcept();
  DRTEST_ASSERT(mock.mock.verify());
  DRTEST_ASSERT(mock.mock.funcVoid0ConstNoexcept().verify());

  mock.mock.funcNonVoid0().push()
      .expects()
      .returns(123)
      .times(1);
  DRTEST_COMPARE(mock.funcNonVoid0(), 123);
  DRTEST_ASSERT(mock.mock.verify());
  DRTEST_ASSERT(mock.mock.funcNonVoid0().verify());

  mock.mock.funcOverloadConst().push()
      .expects()
      .times(1);
  mock.funcOverloadConst();
  DRTEST_ASSERT(mock.mock.verify());
  DRTEST_ASSERT(mock.mock.funcOverloadConst<>().verify());

  mock.mock.funcOverloadConst<drmock::Const>().push()
      .expects()
      .times(1);
  const_cast<const ExampleMock<int, float, double>&>(mock).funcOverloadConst();
  DRTEST_ASSERT(mock.mock.verify());
  DRTEST_ASSERT(mock.mock.funcOverloadConst<drmock::Const>().verify());

  {
    std::vector<std::string> r{ "foo", "bar" };
    mock.mock.funcOverloadConstDifferentReturn().push()
        .expects()
        .returns(r)
        .times(1);
    DRTEST_ASSERT(mock.funcOverloadConstDifferentReturn() == r);
    DRTEST_ASSERT(mock.mock.verify());
    DRTEST_ASSERT(mock.mock.funcOverloadConstDifferentReturn<>().verify());
  }

  {
    std::vector<std::string> r{ "foo", "bar" };
    mock.mock.funcOverloadConstDifferentReturn<drmock::Const>().push()
        .expects()
        .returns(r)
        .times(1);
    auto result = const_cast<
        const ExampleMock<int, float, double>&
      >(mock).funcOverloadConstDifferentReturn();
    DRTEST_ASSERT(result == r);
    DRTEST_ASSERT(mock.mock.verify());
    DRTEST_ASSERT(
        mock.mock.funcOverloadConstDifferentReturn<drmock::Const>().verify()
      );
  }

  {
    mock.mock.funcOverloadParameters().push()
          .expects()
          .times(1);
    mock.funcOverloadParameters();
    DRTEST_ASSERT(mock.mock.verify());
    bool verified = mock.mock.funcOverloadParameters().verify();
    DRTEST_ASSERT(verified);
  }

  {
    std::unordered_map<int, std::string> a1{ { 1, "foo" }, { 2, "bar" } };
    const float a2{12.12f};
    mock.mock.funcOverloadParameters<
        std::unordered_map<int, std::string>,
        const float
      >().push()
          .expects(a1, a2)
          .times(1);
    mock.funcOverloadParameters(a1, a2);
    DRTEST_ASSERT(mock.mock.verify());
    bool verified = mock.mock.funcOverloadParameters<
        std::unordered_map<int, std::string>,
        const float
      >().verify();
    DRTEST_ASSERT(verified);
  }

  {
    std::unordered_map<int, std::string> a1_content{ { 1, "foo" }, { 2, "bar" } };
    auto a1 = std::make_shared<std::unordered_map<int, std::string>>(a1_content);
    mock.mock.funcOverloadParameters<
        const std::shared_ptr<std::unordered_map<int, std::string>>&
      >().push()
          .expects(a1)
          .times(1);
    mock.funcOverloadParameters(a1);
    DRTEST_ASSERT(mock.mock.verify());
    bool verified = mock.mock.funcOverloadParameters<
        const std::shared_ptr<std::unordered_map<int, std::string>>&
      >().verify();
    DRTEST_ASSERT(verified);
  }

  {
    int a1 = 12;
    int a2 = 13;
    mock.mock.funcTemplateParameters<int, int>().push()
        .expects(a1, a2)
        .times(1);
    mock.funcTemplateParameters(a1, a2);
    DRTEST_ASSERT(mock.mock.verify());
    bool verified = mock.mock.funcTemplateParameters<int, int>().verify();
    DRTEST_ASSERT(verified);
  }

  {
    float a1 = 1.2f;
    double a2 = 1.3;
    mock.mock.funcTemplateParameters<float&&, double&&>().push()
        .expects(a1, a2);
    mock.funcTemplateParameters(std::move(a1), std::move(a2));
    DRTEST_ASSERT(mock.mock.verify());
    bool verified = mock.mock.funcTemplateParameters<float&&, double&&>().verify();
    DRTEST_ASSERT(verified);
  }

  {
    int a1 = 12;
    float a2 = 1.3f;
    double a3 = 1.4;
    mock.mock.funcTemplateParameters<const int&, float&&, double&&>().push()
        .expects(a1, a2, a3)
        .returns(true)
        .times(1);
    DRTEST_COMPARE(
        mock.funcTemplateParameters(a1, std::move(a2), std::move(a3)),
        true
      );
    DRTEST_ASSERT(mock.mock.verify());
    bool verified = mock.mock.funcTemplateParameters<const int&, float&&, double&&>().verify();
    DRTEST_ASSERT(verified);
  }

  {
    int a1 = 12;
    int a2 = 13;
    float a3 = 1.4f;
    double a4 = 1.5;
    mock.mock.funcTemplateParameters<
        int&&, const int&, float&&, double&&
      >().push()
          .expects(a1, a2, a3, a4)
          .times(1);
    mock.funcTemplateParameters(std::move(a1), a2, std::move(a3), std::move(a4));
    DRTEST_ASSERT(mock.mock.verify());
    bool verified = mock.mock.funcTemplateParameters<
        int&&, const int&, float&&, double&&
      >().verify();
    DRTEST_ASSERT(verified);
  }

  {
    OuterTypeAlias a1{"foo"};
    outer::inner::IExample<int, float, double>::InnerTypeAlias r{ 1, 2, 3, 4, 5 };
    mock.mock.funcUsingParameter().push()
        .expects(a1)
        .returns(r)
        .times(1);
    DRTEST_ASSERT(mock.funcUsingParameter(a1) == r);
    DRTEST_ASSERT(mock.mock.verify());
    DRTEST_ASSERT(mock.mock.funcUsingParameter().verify());
  }

  {
    int a1 = 12;
    mock.mock.operatorGreater().push()
        .expects(a1)
        .times(1);
    mock.operator>(a1);
    DRTEST_ASSERT(mock.mock.verify());
    DRTEST_ASSERT(mock.mock.operatorGreater().verify());
  }

  {
    int a1 = 12;
    mock.mock.operatorEqual().push()
        .expects(a1)
        .times(1);
    mock.operator==(a1);
    DRTEST_ASSERT(mock.mock.verify());
    DRTEST_ASSERT(mock.mock.operatorEqual().verify());
  }

  {
    int a1 = 12;
    mock.mock.operatorPlus().push()
        .expects(a1)
        .times(1);
    mock.operator+(a1);
    DRTEST_ASSERT(mock.mock.verify());
    DRTEST_ASSERT(mock.mock.operatorPlus().verify());
  }

  {
    const std::string a1{"foo"};
    mock.mock.funcOverloadLvalueRvalueAndConst<const std::string&>().push()
        .expects(a1)
        .times(1);
    mock.funcOverloadLvalueRvalueAndConst(a1);
    DRTEST_ASSERT(mock.mock.verify());
    DRTEST_ASSERT(
        mock.mock.funcOverloadLvalueRvalueAndConst<const std::string&>().verify()
      );
  }

  {
    std::string a1{"foo"};
    mock.mock.funcOverloadLvalueRvalueAndConst<std::string&>().push()
        .expects(a1)
        .times(1);
    mock.funcOverloadLvalueRvalueAndConst(a1);
    DRTEST_ASSERT(mock.mock.verify());
    DRTEST_ASSERT(mock.mock.funcOverloadLvalueRvalueAndConst<std::string&>().verify());
  }

  {
    const std::string a1{"foo"};
    mock.mock.funcOverloadLvalueRvalueAndConst<const std::string&&>().push()
        .expects(a1)
        .times(1);
    mock.funcOverloadLvalueRvalueAndConst(std::move(a1));
    DRTEST_ASSERT(mock.mock.verify());
    DRTEST_ASSERT(
        mock.mock.funcOverloadLvalueRvalueAndConst<const std::string&&>().verify()
      );
  }

  {
    std::string a1{"foo"};
    mock.mock.funcOverloadLvalueRvalueAndConst<std::string&&>().push()
        .expects(a1)
        .times(1);
    mock.funcOverloadLvalueRvalueAndConst(std::move(a1));
    DRTEST_ASSERT(mock.mock.verify());
    DRTEST_ASSERT(mock.mock.funcOverloadLvalueRvalueAndConst<std::string&&>().verify());
  }

  {
    const int* const expects = new int{123};
    mock.mock.funcOddParameters<const int* const>().push()
        .expects(expects)
        .times(1);
    const int* const a1 = new int{123};
    mock.funcOddParameters(a1);
    DRTEST_ASSERT(mock.mock.verify());
    DRTEST_ASSERT(mock.mock.funcOddParameters<const int* const>().verify());
    delete a1;
    delete expects;
  }

  {
    const int* const* expects = new int*{new int{123}};
    mock.mock.funcOddParameters<const int* const* const&&>().push()
        .expects(expects)
        .times(1);
    const int* const* a1 = new int*{new int{123}};
    mock.funcOddParameters(std::move(a1));
    DRTEST_ASSERT(mock.mock.verify());
    DRTEST_ASSERT(mock.mock.funcOddParameters<const int* const* const&&>().verify());
    delete *a1;
    delete a1;
    delete *expects;
    delete expects;
  }

  {
    std::unique_ptr<int> a1 = std::make_unique<int>(12);
    mock.mock.funcNonCopyableArg().push()
        .expects(std::move(a1))
        .times(1);
    a1 = std::make_unique<int>(12);
    mock.funcNonCopyableArg(std::move(a1));
    DRTEST_ASSERT(mock.mock.verify());
    DRTEST_ASSERT(mock.mock.funcNonCopyableArg().verify());
  }

  {
    std::unique_ptr<int> r = std::make_unique<int>(12);
    mock.mock.funcNonCopyableResult().push()
        .expects()
        .returns(std::move(r));
    DRTEST_COMPARE(*mock.funcNonCopyableResult(), 12);
    DRTEST_ASSERT(mock.mock.verify());
    DRTEST_ASSERT(mock.mock.funcNonCopyableResult().verify());
  }

  {
    std::unique_ptr<int> r = std::make_unique<int>(12);
    mock.mock.funcNonCopyableResult().push()
        .expects()
        .returns(std::move(r));
    DRTEST_COMPARE(*mock.funcNonCopyableResult(), 12);
    DRTEST_ASSERT(mock.mock.verify());
    DRTEST_ASSERT(mock.mock.funcNonCopyableResult().verify());
  }
}
