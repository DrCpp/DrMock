#include <unordered_map>
#include <string>
#include <vector>

namespace foo {

class Foo {};

} // namespace foo

namespace ns {

class Interface
{
public:
  static void static_func(int, float);

  int func() volatile
  {
    return 2;
  }

  virtual int virtual_func(foo::Foo) const
  {
    return 1;
  };

  virtual float operator*(float) const = 0;

  virtual void overloaded_params(int, std::vector<float>)
  {
    /* ... */
  }
  virtual void overloaded_params(
      int param0,
      std::unordered_map<std::string, std::shared_ptr<double>> param1
    ) const
  {
    /* ... */
  }

  virtual void reference_qualified(std::string) & = 0;
  virtual void reference_qualified(std::string) && = 0;
};

} // namespace ns
