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

#include "test/Test.h"
#include "mock/AbstractMock.h"

namespace outer { namespace inner {

class Foo : public IFoo
{
public:
  Foo(int x) : x_{x} {}
  void bar() override final { return; }
  bool operator==(const Foo& rhs) const { return x_ == rhs.x_; }
private:
  int x_{0};
};

}} // namespace outer::inner

using namespace outer::inner;

DRTEST_TEST(fails)
{
  {
    // Failure to specify the polymorphic type.
    AbstractMock mock{};
    mock.mock.f().push()
        .expects(std::make_shared<Foo>(0))
        .returns(std::make_shared<Foo>(1))
        .times(1);
    DRTEST_ASSERT_THROW(
        mock.f(std::make_shared<Foo>(3)),
        std::logic_error
      );
  }
  
  {
    // Wrong argument.
    AbstractMock mock{};
    mock.mock.f().polymorphic<std::shared_ptr<Foo>>();
    mock.mock.f().push()
        .expects(std::make_shared<Foo>(0))
        .returns(std::make_shared<Foo>(1))
        .times(1);
    auto ptr = mock.f(std::make_shared<Foo>(2));
    DRTEST_ASSERT(not ptr);
    DRTEST_ASSERT(not mock.mock.verify());
    DRTEST_ASSERT(not mock.mock.f().verify());
  }
}

DRTEST_TEST(success)
{
  {
    AbstractMock mock{};
    mock.mock.f().polymorphic<std::shared_ptr<Foo>>();
    mock.mock.f().push()
        .expects(std::make_shared<Foo>(0))
        .returns(std::make_shared<Foo>(1))
        .times(1);
    auto lhs = mock.f(std::make_shared<Foo>(0));
    auto lhs_cast = std::dynamic_pointer_cast<Foo>(lhs);
    DRTEST_ASSERT(*lhs_cast == Foo{1});
    DRTEST_ASSERT(mock.mock.verify());
    DRTEST_ASSERT(mock.mock.f().verify());
  }
}
