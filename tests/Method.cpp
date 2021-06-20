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
#include "mock/Method.h"

class Dummy {};

using namespace drmock;

DRTEST_TEST(enforceOrderFail)
{
  Method<Dummy, void, int, std::string> m{"test"};
  m.enforce_order(true);
  m.push().expects(1, "foo").times(2);
  m.push().expects(2, "foo").times(1);

  DRTEST_ASSERT(not m.verify());
  m.call(1, "foo");
  DRTEST_ASSERT(not m.verify());
  m.call(2, "foo");
  DRTEST_ASSERT(not m.verify());
  m.call(1, "foo");
  DRTEST_ASSERT(not m.verify());
}

DRTEST_TEST(noEnforceOrder)
{
  Method<Dummy, void, int, std::string> m{"test"};
  m.enforce_order(false);
  m.push().expects(1, "foo").times(2);
  m.push().expects(2, "foo").times(1);

  DRTEST_ASSERT(not m.verify());
  m.call(2, "foo");
  DRTEST_ASSERT(not m.verify());
  m.call(1, "foo");
  DRTEST_ASSERT(not m.verify());
  m.call(1, "foo");
  DRTEST_ASSERT(m.verify());
  m.call(1, "foo");
  DRTEST_ASSERT(not m.verify());
}

DRTEST_TEST(enforceOrderSuccess)
{
  Method<Dummy, void, int, std::string> m{"test"};
  m.enforce_order(true);
  m.push().expects(1, "foo").times(2);
  m.push().expects(2, "foo").times(1);

  DRTEST_ASSERT(not m.verify());
  m.call(1, "foo");
  DRTEST_ASSERT(not m.verify());
  m.call(1, "foo");
  DRTEST_ASSERT(not m.verify());
  m.call(2, "foo");
  DRTEST_ASSERT(m.verify());
}

DRTEST_TEST(nonVoid)
{
  Method<Dummy, int, int, std::string> m{"test"};
  m.enforce_order(true);
  m.push()
      .expects(1, "1")
      .returns(11);
  m.push()
      .expects(2, "2")
      .returns(22);

  DRTEST_ASSERT(not m.verify());
  std::shared_ptr<int> sp = m.call(1, "1");
  DRTEST_ASSERT(not m.verify());
  DRTEST_COMPARE(*sp, 11);

  sp = m.call(2, "2");
  DRTEST_ASSERT(m.verify());
  DRTEST_COMPARE(*sp, 22);
  DRTEST_ASSERT(m.verify());
}

DRTEST_TEST(nonCopyable)
{
  Method<Dummy, std::unique_ptr<int>, int, std::unique_ptr<int>> m{"test"};
  m.enforce_order(true);
  m.push()
      .expects(1, std::make_unique<int>(2))
      .returns(std::make_unique<int>(1));
  m.push()
      .expects(2, std::make_unique<int>(3))
      .returns(std::make_unique<int>(2));

  DRTEST_ASSERT(not m.verify());
  std::shared_ptr<std::unique_ptr<int>> sp = m.call(1, std::make_unique<int>(2));
  DRTEST_ASSERT(not m.verify());
  auto p = std::move(*sp);
  DRTEST_COMPARE(*p, 1);

  sp = m.call(2, std::make_unique<int>(3));
  DRTEST_ASSERT(m.verify());
  p = std::move(*sp);
  DRTEST_COMPARE(*p, 2);
  DRTEST_ASSERT(m.verify());
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

namespace std {

template<>
struct hash<Base>
{
  std::size_t operator()(const Base& t)
  {
    return std::hash<int>{}(t.x());
  }
};

template<>
struct hash<Derived>
{
  std::size_t operator()(const Derived& t)
  {
    return std::hash<int>{}(t.x()) ^ std::hash<int>{}(t.y());
  }
};

}

DRTEST_TEST(polymorphicIo)
{
  {
    Method<Dummy, void, std::shared_ptr<Base>, std::shared_ptr<Base>> m{"test"};
    m.push()
        .expects(std::make_shared<Derived>(1, 2), std::make_shared<Derived>(2, 2));
    m.call(std::make_shared<Derived>(10, 2), std::make_shared<Derived>(10, 2)); // fails
    DRTEST_ASSERT(not m.verify());
  }

  {
    Method<Dummy, void, std::shared_ptr<Base>, std::shared_ptr<Base>> m{"test"};
    m.polymorphic<std::shared_ptr<Derived>, std::shared_ptr<Derived>>();
    m.push()
        .expects(std::make_shared<Derived>(1, 2), std::make_shared<Derived>(2, 2));
    m.call(std::make_shared<Derived>(10, 2), std::make_shared<Derived>(10, 2)); // succeeds
    DRTEST_ASSERT(m.verify());
  }
}

DRTEST_TEST(stateFail)
{
  Method<Dummy, int, int> m{"test"};
  m.state().transition("", "state1", 1);
  m.state().returns("state1", 1);
  m.call(2);
  DRTEST_ASSERT(not m.verify());
}

DRTEST_TEST(stateSuccess)
{
  Method<Dummy, int, int> m{"test"};
  m.state().transition("", "state1", 1);
  m.state().transition("state1", "state2", 2);
  m.state().returns("state1", 1);
  m.call(1);
  DRTEST_ASSERT(m.verify());
  m.call(2);
  DRTEST_ASSERT(not m.verify());
}

DRTEST_TEST(polymorphicState)
{
  {
    Method<Dummy, int, std::shared_ptr<Base>, std::shared_ptr<Base>> m{"test"};
    m.state().transition(
        "",
        "state1",
        std::make_shared<Derived>(1, 2), std::make_shared<Derived>(2, 2)
      );
    m.state().returns("state1", 123);
    m.call(std::make_shared<Derived>(10, 2), std::make_shared<Derived>(10, 2)); // fails
    DRTEST_ASSERT(not m.verify());
  }

  {
    Method<Dummy, int, std::shared_ptr<Base>, std::shared_ptr<Base>> m{"test"};
    m.polymorphic<std::shared_ptr<Derived>, std::shared_ptr<Derived>>();
    m.state().transition(
        "",
        "state1",
        std::make_shared<Derived>(1, 2), std::make_shared<Derived>(2, 2)
      );
    m.state().returns("state1", 123);
    m.call(std::make_shared<Derived>(10, 2), std::make_shared<Derived>(10, 2)); // succeeds
    DRTEST_ASSERT(m.verify());
  }
}

class Interface
{
public:
  virtual ~Interface() = default;
  virtual int f() = 0;
};

class Implementation : public Interface
{
public:
  Implementation(int x): x_{x} {}
  virtual int f() override { return 1; }
  bool operator==(const Implementation& rhs) const { return x_ == rhs.x_; }

private:
  int x_ = 0;
};

DRTEST_TEST(polymorphicPureVirtual)
{
  {
    Method<Dummy, void, std::shared_ptr<Interface>, std::shared_ptr<Interface>> m{"test"};
    m.push()
        .expects(std::make_shared<Implementation>(1), std::make_shared<Implementation>(2))
        .times(1);
    DRTEST_ASSERT_THROW(
        m.call(std::make_shared<Implementation>(1), std::make_shared<Implementation>(2)),
        std::logic_error
      );
  }

  {
    Method<Dummy, void, std::shared_ptr<Interface>, std::shared_ptr<Interface>> m{"test"};
    m.polymorphic<std::shared_ptr<Implementation>, std::shared_ptr<Implementation>>();
    m.push()
        .expects(std::make_shared<Implementation>(1), std::make_shared<Implementation>(2))
        .times(1);
    m.call(std::make_shared<Implementation>(1), std::make_shared<Implementation>(2));
    DRTEST_ASSERT(m.verify());
    m.call(std::make_shared<Implementation>(2), std::make_shared<Implementation>(1));
    DRTEST_ASSERT(not m.verify());
  }
}
