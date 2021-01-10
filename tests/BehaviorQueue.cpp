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

#include <DrMock/test/Test.h>
#include <DrMock/mock/BehaviorQueue.h>

using namespace drmock;

class Dummy
{
public:
  void f(int, float, double) {}
};

using Result = std::pair<
    std::shared_ptr<int>,
    std::shared_ptr<AbstractSignal<Dummy>>
  >;

DRTEST_TEST(isExhausted)
{
  // Empty queue is exhausted.
  BehaviorQueue<Dummy, void, int, std::string> m{};
  DRTEST_ASSERT(m.is_exhausted());

  // Push two behaviors that persist.
  m.push().expects(1, "foo").times(1);
  m.push().expects(2, "foo").times(1);
  DRTEST_ASSERT(not m.is_exhausted());

  // One behavior still persists.
  m.call(1, "foo");
  DRTEST_ASSERT(not m.is_exhausted());

  // No behaviors persist.
  m.call(2, "foo");
  DRTEST_ASSERT(m.is_exhausted());
}

DRTEST_TEST(enforceOrderFail)
{
  BehaviorQueue<Dummy, void, int, std::string> m{};
  m.enforce_order(true);
  m.push().expects(1, "foo").times(2);
  m.push().expects(2, "foo").times(1);

  auto result = m.call(2, "foo");
  DRTEST_ASSERT(std::holds_alternative<std::monostate>(result));
}

DRTEST_TEST(noEnforceOrder)
{
  BehaviorQueue<Dummy, void, int, std::string> m{};
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
  BehaviorQueue<Dummy, void, int, std::string> m{};
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
  BehaviorQueue<Dummy, int, int, std::string> m{};
  m.enforce_order(true);
  m.push()
      .expects(1, "1")
      .returns(11);
  m.push()
      .expects(2, "2")
      .returns(22);
  m.push()
      .expects(3, "3")
      .returns(33)
      .emits(&Dummy::f, 1, 2.3f, 4.5);
  m.push()
      .returns(44)
      .emits(&Dummy::f, 1, 2.3f, 4.5)
      .expects(4, "4");

  auto result = m.call(1, "1");
  std::shared_ptr<int> sp = std::get<Result>(result).first;
  auto signal = std::get<Result>(result).second;
  DRTEST_COMPARE(*sp, 11);
  DRTEST_ASSERT(not signal);

  result = m.call(2, "2");
  sp = std::get<Result>(result).first;
  signal = std::get<Result>(result).second;
  DRTEST_COMPARE(*sp, 22);
  DRTEST_ASSERT(not signal);

  result = m.call(3, "3");
  sp = std::get<Result>(result).first;
  signal = std::get<Result>(result).second;
  DRTEST_COMPARE(*sp, 33);
  DRTEST_ASSERT(signal);

  result = m.call(4, "4");
  sp = std::get<Result>(result).first;
  signal = std::get<Result>(result).second;
  DRTEST_COMPARE(*sp, 44);
  DRTEST_ASSERT(signal);
}

DRTEST_TEST(nonCopyable)
{
  BehaviorQueue<Dummy, std::unique_ptr<int>, int, std::unique_ptr<int>> m{};
  m.enforce_order(true);
  m.push()
      .expects(1, std::make_unique<int>(2))
      .returns(std::make_unique<int>(1));
  m.push()
      .expects(2, std::make_unique<int>(3))
      .returns(std::make_unique<int>(2));

  auto result = m.call(1, std::make_unique<int>(2));
  auto sp = std::get<std::pair<std::shared_ptr<std::unique_ptr<int>>, std::shared_ptr<AbstractSignal<Dummy>>>>(result).first;
  auto p = std::move(*sp);
  DRTEST_COMPARE(*p, 1);

  result = m.call(2, std::make_unique<int>(3));
  sp = std::get<std::pair<std::shared_ptr<std::unique_ptr<int>>, std::shared_ptr<AbstractSignal<Dummy>>>>(result).first;
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
    BehaviorQueue<Dummy, void, std::shared_ptr<Base>, std::shared_ptr<Base>> m{};
    m.push()
        .expects(std::make_shared<Derived>(1, 2), std::make_shared<Derived>(2, 2));
    auto result = m.call(
        std::make_shared<Derived>(10, 2),
        std::make_shared<Derived>(10, 2)
      ); // fails
    DRTEST_ASSERT(std::holds_alternative<std::monostate>(result));
  }

  {
    BehaviorQueue<Dummy, void, std::shared_ptr<Base>, std::shared_ptr<Base>> m{};
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
