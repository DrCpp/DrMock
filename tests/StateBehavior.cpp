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

#include "test/Test.h"
#include "mock/StateBehavior.h"

using namespace drmock;

DRTEST_TEST(noSuchState)
{
  auto so = std::make_shared<StateObject>();
  StateBehavior<int, int> b{so};
  auto result = b.call(1);
  DRTEST_ASSERT(std::holds_alternative<std::monostate>(result));

  b.returns("state", 1);
  result = b.call(2);
  DRTEST_ASSERT(std::holds_alternative<std::monostate>(result));

  b.transition("", "state", 2);
  result = b.call(1);
  DRTEST_ASSERT(std::holds_alternative<std::monostate>(result));
}

DRTEST_TEST(returns)
{
  auto so = std::make_shared<StateObject>();
  StateBehavior<int, int> b{so};
  b.transition("slot1", "", "state1", 1);
  b.transition("slot2", "", "state1", 2);
  b.transition("slot1", "state1", "state3", 3);

  b.returns("slot1", "state1", 1);

  auto result = b.call(1);
  DRTEST_ASSERT(std::holds_alternative<std::shared_ptr<int>>(result));
  auto sp = std::get<std::shared_ptr<int>>(result);
  DRTEST_COMPARE(*sp, 1);

  result = b.call(1);
  DRTEST_ASSERT(std::holds_alternative<std::shared_ptr<int>>(result));
  sp = std::get<std::shared_ptr<int>>(result);
  DRTEST_COMPARE(*sp, 1);

  result = b.call(2);
  DRTEST_ASSERT(std::holds_alternative<std::shared_ptr<int>>(result));
  sp = std::get<std::shared_ptr<int>>(result);
  DRTEST_COMPARE(*sp, 1);

  result = b.call(3);
  DRTEST_ASSERT(std::holds_alternative<std::monostate>(result));
}

DRTEST_TEST(transitionConflictButDifferentInput)
{
  auto so = std::make_shared<StateObject>();
  StateBehavior<int, int> b{so};

  // None of the following should throw.
  b.transition("", "state1", 1);
  b.transition("", "state2", 2);
  b.transition("", "state3", 3);
}

DRTEST_TEST(transitionWildcard)
{
  auto so = std::make_shared<StateObject>();
  StateBehavior<int, int> b{so};

  // None of the following should throw.
  b.transition("", "state1", 0);
  b.transition("*", "state2", 0);
  b.transition("abc", "state3", 0);
}

DRTEST_TEST(multipleArguments)
{
  auto so = std::make_shared<StateObject>();
  StateBehavior<int, int, float, double> b{so};
  b.transition("", "state1", 1, 2.3f, 4.5);
  b.transition("", "state2", 1, 2.3f, 9.9);
  b.transition("", "state3", 9, 2.3f, 4.5);
  b.transition("", "state4", 1, 9.9f, 4.5);
  b.transition("state1", "", 0, 0.0f, 0.0);  // reset
  b.transition("state2", "", 0, 0.0f, 0.0);
  b.transition("state3", "", 0, 0.0f, 0.0);
  b.transition("state4", "", 0, 0.0f, 0.0);
  
  b.returns("", "state1", 1);
  b.returns("", "state2", 2);
  b.returns("", "state3", 3);
  b.returns("", "state4", 4);

  // Wrong argument.
  auto result = b.call(0, 2.3f, 4.5);
  DRTEST_ASSERT(std::holds_alternative<std::monostate>(result));
  b.call(0, 0.0f, 0.0);
  result = b.call(1, 0.0f, 4.5);
  DRTEST_ASSERT(std::holds_alternative<std::monostate>(result));
  b.call(0, 0.0f, 0.0);
  result = b.call(1, 2.3f, 0.0);
  DRTEST_ASSERT(std::holds_alternative<std::monostate>(result));
  b.call(0, 0.0f, 0.0);

  // Correct argument.
  result = b.call(1, 2.3f, 4.5);
  DRTEST_ASSERT(std::holds_alternative<std::shared_ptr<int>>(result));
  auto sp = std::get<std::shared_ptr<int>>(result);
  DRTEST_COMPARE(*sp, 1);
  b.call(0, 0.0f, 0.0);

  result = b.call(1, 2.3f, 9.9);
  DRTEST_ASSERT(std::holds_alternative<std::shared_ptr<int>>(result));
  sp = std::get<std::shared_ptr<int>>(result);
  DRTEST_COMPARE(*sp, 2);
  b.call(0, 0.0f, 0.0);

  result = b.call(9, 2.3f, 4.5);
  DRTEST_ASSERT(std::holds_alternative<std::shared_ptr<int>>(result));
  sp = std::get<std::shared_ptr<int>>(result);
  DRTEST_COMPARE(*sp, 3);
  b.call(0, 0.0f, 0.0);

  result = b.call(1, 9.9f, 4.5);
  DRTEST_ASSERT(std::holds_alternative<std::shared_ptr<int>>(result));
  sp = std::get<std::shared_ptr<int>>(result);
  DRTEST_COMPARE(*sp, 4);
}

DRTEST_TEST(returnsVoid)
{
  auto so = std::make_shared<StateObject>();
  StateBehavior<void, int> b{so};

  auto result = b.call(1);
  DRTEST_ASSERT(std::holds_alternative<std::shared_ptr<void>>(result));
  auto sp = std::get<std::shared_ptr<void>>(result);
  DRTEST_ASSERT(not sp); // It's a void function
}

DRTEST_TEST(throws)
{
  auto so = std::make_shared<StateObject>();
  StateBehavior<int, int> b{so};
  b.transition("slot1", "", "state1", 1);
  b.transition("slot2", "", "state1", 2);

  b.throws("slot1", "state1", std::runtime_error{""});

  auto result = b.call(1);
  DRTEST_ASSERT(std::holds_alternative<std::exception_ptr>(result));
  auto ex = std::get<std::exception_ptr>(result);
  DRTEST_ASSERT_THROW(
      std::rethrow_exception(ex),
      std::runtime_error
    );
}

DRTEST_TEST(throwsVoid)
{
  auto so = std::make_shared<StateObject>();
  StateBehavior<void, int> b{so};

  b.throws("", std::runtime_error{""});

  auto result = b.call(123);
  DRTEST_ASSERT(std::holds_alternative<std::exception_ptr>(result));
  auto ex = std::get<std::exception_ptr>(result);
  DRTEST_ASSERT_THROW(
      std::rethrow_exception(ex),
      std::runtime_error
    );
}

DRTEST_TEST(multipleResultStates)
{
  auto so = std::make_shared<StateObject>();
  StateBehavior<int, int> b{so};
  b.transition("slot1", "", "state1", 1);
  b.transition("slot2", "", "state1", 2);

  b.returns("slot1", "state1", 1);
  b.returns("slot1", "state2", 1); // Ok, same slot
  DRTEST_ASSERT_THROW(
      b.returns("slot2", "state3", 1), // Not ok, different slot
      std::exception
    );
}

DRTEST_TEST(wildcardState)
{
  auto so = std::make_shared<StateObject>();
  StateBehavior<int, int> b{so};
  b.transition("*", "state1", 1);
  b.returns("state1", 1);

  auto result = b.call(1);
  DRTEST_ASSERT(std::holds_alternative<std::shared_ptr<int>>(result));
  auto sp = std::get<std::shared_ptr<int>>(result);
  DRTEST_COMPARE(*sp, 1);

  result = b.call(1);
  DRTEST_ASSERT(std::holds_alternative<std::shared_ptr<int>>(result));
  sp = std::get<std::shared_ptr<int>>(result);
  DRTEST_COMPARE(*sp, 1);

  result = b.call(2);
  DRTEST_ASSERT(std::holds_alternative<std::shared_ptr<int>>(result));
  sp = std::get<std::shared_ptr<int>>(result);
  DRTEST_COMPARE(*sp, 1);
}

DRTEST_TEST(multiple)
{
  auto so = std::make_shared<StateObject>();
  so->set("on");

  StateBehavior<void, bool> b1{so};
  StateBehavior<bool> b2{so};

  b2.returns("on", true)
    .returns("off", false);

  b1.transition("off", "on", true) // transition on void method implies returns()
    .transition("on", "off", false);

  auto result = b2.call();
  DRTEST_ASSERT(std::holds_alternative<std::shared_ptr<bool>>(result));
  auto sp = std::get<std::shared_ptr<bool>>(result);
  DRTEST_ASSERT(*sp);

  b1.call(false);

  result = b2.call();
  DRTEST_ASSERT(std::holds_alternative<std::shared_ptr<bool>>(result));
  sp = std::get<std::shared_ptr<bool>>(result);
  DRTEST_ASSERT(not *sp);

  result = b2.call();
  DRTEST_ASSERT(std::holds_alternative<std::shared_ptr<bool>>(result));
  sp = std::get<std::shared_ptr<bool>>(result);
  DRTEST_ASSERT(not *sp);

  b1.call(true);

  result = b2.call();
  DRTEST_ASSERT(std::holds_alternative<std::shared_ptr<bool>>(result));
  sp = std::get<std::shared_ptr<bool>>(result);
  DRTEST_ASSERT(*sp);
}

DRTEST_TEST(string)
{
  auto so = std::make_shared<StateObject>();
  StateBehavior<int, std::string> b{so};
  auto result = b.call("test");
  DRTEST_ASSERT(std::holds_alternative<std::monostate>(result));

  b.returns("state", 1);
  result = b.call("foo");
  DRTEST_ASSERT(std::holds_alternative<std::monostate>(result));

  b.transition("", "state", "foo");
  result = b.call("foo");
  DRTEST_ASSERT(std::holds_alternative<std::shared_ptr<int>>(result));
  auto sp = std::get<std::shared_ptr<int>>(result);
  DRTEST_COMPARE(*sp, 1);
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

template<>
struct std::hash<Base>
{
  std::size_t operator()(const Base& a)
  {
    return a.x();
  }
};

template<>
struct std::hash<Derived>
{
  std::size_t operator()(const Derived& a)
  {
    return a.y();
  }
};

DRTEST_TEST(polymorphic)
{
  auto so = std::make_shared<StateObject>();
  StateBehavior<int, std::shared_ptr<Base>, std::shared_ptr<Base>> b{so};
  b.transition("", "state1", std::make_shared<Derived>(1, 2), std::make_shared<Derived>(2, 2));
  
  b.returns("state1", 1);

  // No polymorphism.
  auto result = b.call(std::make_shared<Derived>(10, 2), std::make_shared<Derived>(10, 2));
  DRTEST_ASSERT(std::holds_alternative<std::monostate>(result));

  b.polymorphic<std::shared_ptr<Derived>, std::shared_ptr<Derived>>();
  result = b.call(std::make_shared<Derived>(10, 2), std::make_shared<Derived>(10, 2));
  DRTEST_ASSERT(std::holds_alternative<std::shared_ptr<int>>(result));
  auto sp = std::get<std::shared_ptr<int>>(result);
  DRTEST_COMPARE(*sp, 1);

  b.polymorphic<std::shared_ptr<Base>, std::shared_ptr<Derived>>();
  result = b.call(std::make_shared<Derived>(1, 10), std::make_shared<Derived>(10, 2));
  DRTEST_ASSERT(std::holds_alternative<std::shared_ptr<int>>(result));
  sp = std::get<std::shared_ptr<int>>(result);
  DRTEST_COMPARE(*sp, 1);

  b.polymorphic<std::shared_ptr<Derived>, std::shared_ptr<Base>>();
  result = b.call(std::make_shared<Derived>(10, 2), std::make_shared<Derived>(2, 10));
  DRTEST_ASSERT(std::holds_alternative<std::shared_ptr<int>>(result));
  sp = std::get<std::shared_ptr<int>>(result);
  DRTEST_COMPARE(*sp, 1);
}

DRTEST_TEST(transitionFailure)
{
  auto so = std::make_shared<StateObject>();
  StateBehavior<int, int> b{so};
  b.transition("", "state1", 0);
  DRTEST_ASSERT_THROW(
      (b.transition("", "state2", 0)),
      std::runtime_error
    );
}

DRTEST_TEST(wildcardOverride)
{
  auto so = std::make_shared<StateObject>();
  StateBehavior<int, int> b{so};
  b.transition("*", "state2", 0);
  b.transition("", "state1", 0);
  b.returns("state1", 1);
  b.returns("state2", 2);

  auto result = b.call(0);
  DRTEST_ASSERT(std::holds_alternative<std::shared_ptr<int>>(result));
  auto cast = std::get<std::shared_ptr<int>>(result);
  DRTEST_COMPARE(*cast, 1);

  result = b.call(0);
  DRTEST_ASSERT(std::holds_alternative<std::shared_ptr<int>>(result));
  cast = std::get<std::shared_ptr<int>>(result);
  DRTEST_COMPARE(*cast, 2);
}

DRTEST_TEST(wildcardAsTargetState)
{
  auto so = std::make_shared<StateObject>();
  StateBehavior<int, int> b{so};
  DRTEST_ASSERT_THROW(
      b.transition("", "*", 0),
      std::runtime_error
    );
}

DRTEST_TEST(pushingMultipleWildcardTransitions)
{
  auto so = std::make_shared<StateObject>();
  StateBehavior<int, int> b{so};
  b.transition("*", "state1", 0);
  DRTEST_ASSERT_THROW(
      b.transition("*", "state1", 0),
      std::runtime_error
    );
}
