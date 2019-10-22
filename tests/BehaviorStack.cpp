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

#include <string>
#include <memory>

#include "test/Test.h"
#include "mock/BehaviorStack.h"

using namespace drmock;

DRTEST_TEST(enforceOrderFail)
{
  BehaviorStack<void, int, std::string> m{};
  m.enforce_order(true);
  m.push().expects(1, "foo").times(2);
  m.push().expects(2, "foo").times(1);

  auto result = m.call(2, "foo");
  DRTEST_ASSERT(std::holds_alternative<std::monostate>(result));
}

DRTEST_TEST(noEnforceOrder)
{
  BehaviorStack<void, int, std::string> m{};
  m.enforce_order(false);
  m.push().expects(1, "foo").times(2);
  m.push().expects(2, "foo").times(1);

  auto result = m.call(2, "foo");
  DRTEST_ASSERT(not std::holds_alternative<std::monostate>(result));
  result = m.call(1, "foo");
  DRTEST_ASSERT(not std::holds_alternative<std::monostate>(result));
  result = m.call(1, "foo");
  DRTEST_ASSERT(not std::holds_alternative<std::monostate>(result));
  result = m.call(1, "foo");
  DRTEST_ASSERT(std::holds_alternative<std::monostate>(result));
}

DRTEST_TEST(enforceOrderSuccess)
{
  BehaviorStack<void, int, std::string> m{};
  m.enforce_order(true);
  m.push().expects(1, "foo").times(2);
  m.push().expects(2, "foo").times(1);

  auto result = m.call(1, "foo");
  DRTEST_ASSERT(not std::holds_alternative<std::monostate>(result));
  m.call(1, "foo");
  DRTEST_ASSERT(not std::holds_alternative<std::monostate>(result));
  m.call(2, "foo");
  DRTEST_ASSERT(not std::holds_alternative<std::monostate>(result));
}

DRTEST_TEST(nonVoid)
{
  BehaviorStack<int, int, std::string> m{};
  m.enforce_order(true);
  m.push()
      .expects(1, "1")
      .returns(11);
  m.push()
      .expects(2, "2")
      .returns(22);

  auto result = m.call(1, "1");
  std::shared_ptr<int> sp = std::get<std::shared_ptr<int>>(result);
  DRTEST_COMPARE(*sp, 11);

  result = m.call(2, "2");
  sp = std::get<std::shared_ptr<int>>(result);
  DRTEST_COMPARE(*sp, 22);
}

DRTEST_TEST(nonCopyable)
{
  BehaviorStack<std::unique_ptr<int>, int, std::unique_ptr<int>> m{};
  m.enforce_order(true);
  m.push()
      .expects(1, std::make_unique<int>(2))
      .returns(std::make_unique<int>(1));
  m.push()
      .expects(2, std::make_unique<int>(3))
      .returns(std::make_unique<int>(2));

  auto result = m.call(1, std::make_unique<int>(2));
  auto sp = std::get<std::shared_ptr<std::unique_ptr<int>>>(result);
  auto p = std::move(*sp);
  DRTEST_COMPARE(*p, 1);

  result = m.call(2, std::make_unique<int>(3));
  sp = std::get<std::shared_ptr<std::unique_ptr<int>>>(result);
  p = std::move(*sp);
  DRTEST_COMPARE(*p, 2);
}

class Base
{
public:
  Base(int x) : x_{x} {}
  virtual ~Base() = default;
  bool operator==(const Base& other) const { return x_ == other.x_; }
  int x() const { return x_; }
private:
  int x_;
};

class Derived : public Base
{
public:
  Derived(int x, int y) : Base{x}, y_{y} {}
  bool operator==(const Derived& other) const { return y_ == other.y_; }
  int y() const { return y_; }
private:
  int y_;
};

DRTEST_TEST(polymorphic)
{
  {
    BehaviorStack<void, std::shared_ptr<Base>, std::shared_ptr<Base>> m{};
    m.push()
        .expects(std::make_shared<Derived>(1, 2), std::make_shared<Derived>(2, 2));
    auto result = m.call(
        std::make_shared<Derived>(10, 2),
        std::make_shared<Derived>(10, 2)
      ); // fails
    DRTEST_ASSERT(std::holds_alternative<std::monostate>(result));
  }

  {
    BehaviorStack<void, std::shared_ptr<Base>, std::shared_ptr<Base>> m{};
    m.polymorphic<std::shared_ptr<Derived>, std::shared_ptr<Derived>>();
    m.push()
        .expects(std::make_shared<Derived>(1, 2), std::make_shared<Derived>(2, 2));
    auto result = m.call(
        std::make_shared<Derived>(10, 2),
        std::make_shared<Derived>(10, 2)
      ); // succeeds
    DRTEST_ASSERT(not std::holds_alternative<std::monostate>(result));
  }
}
