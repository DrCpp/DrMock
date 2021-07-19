#ifndef DRMOCK_MOCK_IMPLEMENTATIONSInterface
#define DRMOCK_MOCK_IMPLEMENTATIONSInterface

#include <DrMock/Mock.h>
#include "/Users/malte/drmock/docs/example.h"

extern template class ::drmock::Method<ns::Interface, void, int, std :: vector < float >>;
extern template class ::drmock::Method<ns::Interface, void, std :: string>;
extern template class ::drmock::Method<ns::Interface, float, float>;
extern template class ::drmock::Method<ns::Interface, int, foo :: Foo>;
extern template class ::drmock::Method<ns::Interface, void, int, std :: unordered_map < std :: string , std :: shared_ptr < double >>>;
namespace ns { namespace mock {

class DRMOCK_OBJECTInterface
{
  friend class ns::Interface;
  std::shared_ptr<::drmock::StateObject> DRMOCK_STATE_OBJECT_{std::make_shared<::drmock::StateObject>()};
  std::shared_ptr<::drmock::Method<ns::Interface, int, foo :: Foo>> DRMOCK_METHOD_PTRvirtual_func_0{std::make_shared<::drmock::Method<ns::Interface, int, foo :: Foo>>("virtual_func", DRMOCK_STATE_OBJECT_)};
  std::shared_ptr<::drmock::Method<ns::Interface, float, float>> DRMOCK_METHOD_PTRoperatorAst_0{std::make_shared<::drmock::Method<ns::Interface, float, float>>("operator*", DRMOCK_STATE_OBJECT_)};
  std::shared_ptr<::drmock::Method<ns::Interface, void, int, std :: vector < float >>> DRMOCK_METHOD_PTRoverloaded_params_0{std::make_shared<::drmock::Method<ns::Interface, void, int, std :: vector < float >>>("overloaded_params", DRMOCK_STATE_OBJECT_)};
  std::shared_ptr<::drmock::Method<ns::Interface, void, int, std :: unordered_map < std :: string , std :: shared_ptr < double >>>> DRMOCK_METHOD_PTRoverloaded_params_1{std::make_shared<::drmock::Method<ns::Interface, void, int, std :: unordered_map < std :: string , std :: shared_ptr < double >>>>("overloaded_params", DRMOCK_STATE_OBJECT_)};
  std::shared_ptr<::drmock::Method<ns::Interface, void, std :: string>> DRMOCK_METHOD_PTRreference_qualified_0{std::make_shared<::drmock::Method<ns::Interface, void, std :: string>>("reference_qualified", DRMOCK_STATE_OBJECT_)};
  std::shared_ptr<::drmock::Method<ns::Interface, void, std :: string>> DRMOCK_METHOD_PTRreference_qualified_1{std::make_shared<::drmock::Method<ns::Interface, void, std :: string>>("reference_qualified", DRMOCK_STATE_OBJECT_)};

public:
  ::drmock::Controller control{{DRMOCK_METHOD_PTRvirtual_func_0, DRMOCK_METHOD_PTRoperatorAst_0, DRMOCK_METHOD_PTRoverloaded_params_0, DRMOCK_METHOD_PTRoverloaded_params_1, DRMOCK_METHOD_PTRreference_qualified_0, DRMOCK_METHOD_PTRreference_qualified_1}, DRMOCK_STATE_OBJECT_};

private:
  auto & DRMOCK_DISPATCHvirtual_func(::drmock::TypeContainer<foo :: Foo, ::drmock::Const>)
  {
    return *DRMOCK_METHOD_PTRvirtual_func_0;
  }
  auto & DRMOCK_DISPATCHoperatorAst(::drmock::TypeContainer<float, ::drmock::Const>)
  {
    return *DRMOCK_METHOD_PTRoperatorAst_0;
  }
  auto & DRMOCK_DISPATCHoverloaded_params(::drmock::TypeContainer<int, std :: vector < float >>)
  {
    return *DRMOCK_METHOD_PTRoverloaded_params_0;
  }
  auto & DRMOCK_DISPATCHoverloaded_params(::drmock::TypeContainer<int, std :: unordered_map < std :: string , std :: shared_ptr < double >>, ::drmock::Const>)
  {
    return *DRMOCK_METHOD_PTRoverloaded_params_1;
  }
  auto & DRMOCK_DISPATCHreference_qualified(::drmock::TypeContainer<std :: string, ::drmock::LValueRef>)
  {
    return *DRMOCK_METHOD_PTRreference_qualified_0;
  }
  auto & DRMOCK_DISPATCHreference_qualified(::drmock::TypeContainer<std :: string, ::drmock::RValueRef>)
  {
    return *DRMOCK_METHOD_PTRreference_qualified_1;
  }

public:
  auto & virtual_func()
  {
    return DRMOCK_DISPATCHvirtual_func(::drmock::TypeContainer<foo :: Foo, ::drmock::Const>{});
  }
  auto & operatorAst()
  {
    return DRMOCK_DISPATCHoperatorAst(::drmock::TypeContainer<float, ::drmock::Const>{});
  }
  template<typename ... DRMOCK_Ts>
  auto & overloaded_params()
  {
    return DRMOCK_DISPATCHoverloaded_params(::drmock::TypeContainer<DRMOCK_Ts ...>{});
  }
  template<typename ... DRMOCK_Ts>
  auto & reference_qualified()
  {
    return DRMOCK_DISPATCHreference_qualified(::drmock::TypeContainer<std :: string, DRMOCK_Ts ...>{});
  }
};

}} // namespace ns::mock

namespace ns { namespace mock {

class MockImpl : public ns::Interface
{

public:
  template<typename ... DRMOCK_FORWARDING_CTOR_TS>
  MockImpl(DRMOCK_FORWARDING_CTOR_TS&&... ts) : ns::Interface{std::forward<DRMOCK_FORWARDING_CTOR_TS>(ts)...}
  {
    mock.virtual_func().parent(this);
    mock.operatorAst().parent(this);
    mock.template overloaded_params<int, std :: vector < float >>().parent(this);
    mock.template overloaded_params<int, std :: unordered_map < std :: string , std :: shared_ptr < double >>, ::drmock::Const>().parent(this);
    mock.template reference_qualified<::drmock::LValueRef>().parent(this);
    mock.template reference_qualified<::drmock::RValueRef>().parent(this);
  }
  mutable ns::mock::DRMOCK_OBJECTInterface mock{};
  int virtual_func(foo :: Foo a0) const override
  {
    auto& result = *mock.virtual_func().call(std::forward<foo :: Foo>(a0));
    return std::forward<int>(::drmock::move_if_not_copy_constructible(result));
  }
  float operator*(float a0) const override
  {
    auto& result = *mock.operatorAst().call(std::forward<float>(a0));
    return std::forward<float>(::drmock::move_if_not_copy_constructible(result));
  }
  void overloaded_params(int a0, std :: vector < float > a1) override
  {
    mock.template overloaded_params<int, std :: vector < float >>().call(std::forward<int>(a0), std::forward<std :: vector < float >>(a1));
  }
  void overloaded_params(int a0, std :: unordered_map < std :: string , std :: shared_ptr < double >> a1) const override
  {
    mock.template overloaded_params<int, std :: unordered_map < std :: string , std :: shared_ptr < double >>, ::drmock::Const>().call(std::forward<int>(a0), std::forward<std :: unordered_map < std :: string , std :: shared_ptr < double >>>(a1));
  }
  void reference_qualified(std :: string a0)& override
  {
    mock.template reference_qualified<::drmock::LValueRef>().call(std::forward<std :: string>(a0));
  }
  void reference_qualified(std :: string a0)&& override
  {
    mock.template reference_qualified<::drmock::RValueRef>().call(std::forward<std :: string>(a0));
  }
};

}} // namespace ns::mock

#endif /* DRMOCK_MOCK_IMPLEMENTATIONSInterface */