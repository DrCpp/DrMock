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

#include <DrMock/Test.h>
#include <DrMock/mock/StateBehavior.h>

// FIXME Check that the correct arguments are forwarded to the signal in
// the following tests:
//
// emits
// returnsAndEmits

using namespace drmock;

class Dummy
{
public:
  // We're using this method as fake signal.
  void f(int, float&, const double&) {}
};

template<typename ReturnType>
using Result = std::pair<
    std::shared_ptr<ReturnType>,
    std::shared_ptr<AbstractSignal<Dummy>>
  >;

DRTEST_TEST(noSuchState)
{
  auto so = std::make_shared<StateObject>();
  StateBehavior<Dummy, int, int> b{so};
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
  StateBehavior<Dummy, int, int> b{so};
  b.transition("slot1", "", "state1", 1);
  b.transition("slot2", "", "state1", 2);
  b.transition("slot1", "state1", "state3", 3);

  b.returns("slot1", "state1", 1);

  auto result = b.call(1);
  DRTEST_ASSERT(std::holds_alternative<Result<int>>(result));
  auto sp = std::get<Result<int>>(result).first;
  auto signal = std::get<Result<int>>(result).second;
  DRTEST_COMPARE(*sp, 1);
  DRTEST_ASSERT(not signal);

  result = b.call(1);
  DRTEST_ASSERT(std::holds_alternative<Result<int>>(result));
  sp = std::get<Result<int>>(result).first;
  signal = std::get<Result<int>>(result).second;
  DRTEST_COMPARE(*sp, 1);
  DRTEST_ASSERT(not signal);

  result = b.call(2);
  DRTEST_ASSERT(std::holds_alternative<Result<int>>(result));
  sp = std::get<Result<int>>(result).first;
  signal = std::get<Result<int>>(result).second;
  DRTEST_COMPARE(*sp, 1);
  DRTEST_ASSERT(not signal);

  result = b.call(3);
  DRTEST_ASSERT(std::holds_alternative<std::monostate>(result));
}

DRTEST_TEST(emits)
{
  auto so = std::make_shared<StateObject>();
  StateBehavior<Dummy, void, int> b{so};
  b.transition("slot1", "", "state1", 1);
  b.transition("slot2", "", "state1", 2);
  b.transition("slot1", "state1", "state3", 3);

  float f = 1.23f;
  double x = 4.56;
  // Note: Manually specifying the template parameters is necessary here,
  // because `x` is not const.
  b.emits<int, float&, const double&>("slot1", "state1", &Dummy::f, 123, f, x);

  auto result = b.call(1);
  DRTEST_ASSERT(std::holds_alternative<Result<void>>(result));
  auto signal = std::get<Result<void>>(result).second;
  DRTEST_ASSERT(signal);

  result = b.call(1);
  DRTEST_ASSERT(std::holds_alternative<Result<void>>(result));
  signal = std::get<Result<void>>(result).second;
  DRTEST_ASSERT(signal);

  result = b.call(2);
  DRTEST_ASSERT(std::holds_alternative<Result<void>>(result));
  signal = std::get<Result<void>>(result).second;
  DRTEST_ASSERT(signal);

  result = b.call(3);
  DRTEST_ASSERT(std::holds_alternative<Result<void>>(result));
  signal = std::get<Result<void>>(result).second;
  DRTEST_ASSERT(not signal);
}

DRTEST_TEST(returnsAndEmits)
{
  auto so = std::make_shared<StateObject>();
  StateBehavior<Dummy, int, int> b{so};
  b.transition("slot1", "", "state1", 1);
  b.transition("slot2", "", "state1", 2);
  b.transition("slot1", "state1", "state3", 3);
  b.transition("slot1", "state3", "state4", 3);

  float f = 1.23f;
  double x = 4.56;
  b.returns("slot1", "state1", 1);
  // Note: Manually specifying the template parameters is necessary here,
  // because `x` is not const.
  b.emits<int, float&, const double&>("slot1", "state1", &Dummy::f, 123, f, x);

  // Note: We've reversed the order of the emit/return value.
  b.emits<int, float&, const double&>("slot1", "state3", &Dummy::f, 123, f, x);
  b.returns("slot1", "state3", 3);

  auto result = b.call(1);
  DRTEST_ASSERT(std::holds_alternative<Result<int>>(result));
  auto sp = std::get<Result<int>>(result).first;
  auto signal = std::get<Result<int>>(result).second;
  DRTEST_COMPARE(*sp, 1);
  DRTEST_ASSERT(signal);

  result = b.call(3);
  DRTEST_ASSERT(std::holds_alternative<Result<int>>(result));
  sp = std::get<Result<int>>(result).first;
  signal = std::get<Result<int>>(result).second;
  DRTEST_COMPARE(*sp, 3);
  DRTEST_ASSERT(signal);

  result = b.call(3);
  DRTEST_ASSERT(std::holds_alternative<std::monostate>(result));
}

DRTEST_TEST(returnsAndEmitsFailure)
{
  // Test that if and emit is set, but no return value, then an
  // std::monotstate is returned.

  auto so = std::make_shared<StateObject>();
  StateBehavior<Dummy, int, int> b{so};
  float f = 1.23f;
  double x = 4.56;
  b.emits<int, float&, const double&>("slot1", "state1", &Dummy::f, 123, f, x);

  auto result = b.call(1);
  DRTEST_ASSERT(std::holds_alternative<std::monostate>(result));
}

DRTEST_TEST(transitionConflictButDifferentInput)
{
  auto so = std::make_shared<StateObject>();
  StateBehavior<Dummy, int, int> b{so};

  // None of the following should throw.
  b.transition("", "state1", 1);
  b.transition("", "state2", 2);
  b.transition("", "state3", 3);
}

DRTEST_TEST(transitionWildcard)
{
  auto so = std::make_shared<StateObject>();
  StateBehavior<Dummy, int, int> b{so};

  // None of the following should throw.
  b.transition("", "state1", 0);
  b.transition("*", "state2", 0);
  b.transition("abc", "state3", 0);
}

DRTEST_TEST(multipleArguments)
{
  auto so = std::make_shared<StateObject>();
  StateBehavior<Dummy, int, int, float, double> b{so};
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
  DRTEST_ASSERT(std::holds_alternative<Result<int>>(result));
  auto sp = std::get<Result<int>>(result).first;
  DRTEST_COMPARE(*sp, 1);
  b.call(0, 0.0f, 0.0);

  result = b.call(1, 2.3f, 9.9);
  DRTEST_ASSERT(std::holds_alternative<Result<int>>(result));
  sp = std::get<Result<int>>(result).first;
  DRTEST_COMPARE(*sp, 2);
  b.call(0, 0.0f, 0.0);

  result = b.call(9, 2.3f, 4.5);
  DRTEST_ASSERT(std::holds_alternative<Result<int>>(result));
  sp = std::get<Result<int>>(result).first;
  DRTEST_COMPARE(*sp, 3);
  b.call(0, 0.0f, 0.0);

  result = b.call(1, 9.9f, 4.5);
  DRTEST_ASSERT(std::holds_alternative<Result<int>>(result));
  sp = std::get<Result<int>>(result).first;
  DRTEST_COMPARE(*sp, 4);
}

DRTEST_TEST(returnsVoid)
{
  auto so = std::make_shared<StateObject>();
  StateBehavior<Dummy, void, int> b{so};

  auto result = b.call(1);
  DRTEST_ASSERT(std::holds_alternative<Result<void>>(result));
  auto sp = std::get<Result<void>>(result).first;
  DRTEST_ASSERT(not sp); // It's a void function
}

DRTEST_TEST(throws)
{
  auto so = std::make_shared<StateObject>();
  StateBehavior<Dummy, int, int> b{so};
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
  StateBehavior<Dummy, void, int> b{so};

  b.throws("", std::runtime_error{""});

  auto result = b.call(123);
  DRTEST_ASSERT(std::holds_alternative<std::exception_ptr>(result));
  auto ex = std::get<std::exception_ptr>(result);
  DRTEST_ASSERT_THROW(
      std::rethrow_exception(ex),
      std::runtime_error
    );
}

DRTEST_TEST(multipleResultSlots)
{
  auto so = std::make_shared<StateObject>();
  StateBehavior<Dummy, int, int> b{so};
  b.transition("slot1", "", "state1", 1);
  b.transition("slot2", "", "state1", 2);

  b.returns("slot1", "state1", 1);
  b.returns("slot1", "state2", 1); // Ok, same slot
  DRTEST_ASSERT_THROW(
      b.returns("slot2", "state3", 1), // Not ok, different slot
      std::exception
    );
}

DRTEST_TEST(sameResultState)
{
  // Check that using the same result state twice result in an error.
  StateBehavior<Dummy, int, int> b{};
  b.transition("slot1", "", "state1", 1);
  b.transition("slot2", "", "state1", 2);

  b.returns("slot1", "state1", 1);
  b.returns("slot1", "state2", 1); // Ok, same slot
  DRTEST_ASSERT_THROW(
      b.returns("slot1", "state1", 1), // Not ok, different slot
      std::exception
    );
}

DRTEST_TEST(wildcardState)
{
  auto so = std::make_shared<StateObject>();
  StateBehavior<Dummy, int, int> b{so};
  b.transition("*", "state1", 1);
  b.returns("state1", 1);

  auto result = b.call(1);
  DRTEST_ASSERT(std::holds_alternative<Result<int>>(result));
  auto sp = std::get<Result<int>>(result).first;
  DRTEST_COMPARE(*sp, 1);

  result = b.call(1);
  DRTEST_ASSERT(std::holds_alternative<Result<int>>(result));
  sp = std::get<Result<int>>(result).first;
  DRTEST_COMPARE(*sp, 1);

  result = b.call(2);
  DRTEST_ASSERT(std::holds_alternative<Result<int>>(result));
  sp = std::get<Result<int>>(result).first;
  DRTEST_COMPARE(*sp, 1);
}

DRTEST_TEST(multiple)
{
  auto so = std::make_shared<StateObject>();
  so->set("on");

  StateBehavior<Dummy, void, bool> b1{so};
  StateBehavior<Dummy, bool> b2{so};

  b2.returns("on", true)
    .returns("off", false);

  b1.transition("off", "on", true) // transition on void method implies returns()
    .transition("on", "off", false);

  auto result = b2.call();
  DRTEST_ASSERT(std::holds_alternative<Result<bool>>(result));
  auto sp = std::get<Result<bool>>(result).first;
  DRTEST_ASSERT(*sp);

  b1.call(false);

  result = b2.call();
  DRTEST_ASSERT(std::holds_alternative<Result<bool>>(result));
  sp = std::get<Result<bool>>(result).first;
  DRTEST_ASSERT(not *sp);

  result = b2.call();
  DRTEST_ASSERT(std::holds_alternative<Result<bool>>(result));
  sp = std::get<Result<bool>>(result).first;
  DRTEST_ASSERT(not *sp);

  b1.call(true);

  result = b2.call();
  DRTEST_ASSERT(std::holds_alternative<Result<bool>>(result));
  sp = std::get<Result<bool>>(result).first;
  DRTEST_ASSERT(*sp);
}

DRTEST_TEST(string)
{
  auto so = std::make_shared<StateObject>();
  StateBehavior<Dummy, int, std::string> b{so};
  auto result = b.call("test");
  DRTEST_ASSERT(std::holds_alternative<std::monostate>(result));

  b.returns("state", 1);
  result = b.call("foo");
  DRTEST_ASSERT(std::holds_alternative<std::monostate>(result));

  b.transition("", "state", "foo");
  result = b.call("foo");
  DRTEST_ASSERT(std::holds_alternative<Result<int>>(result));
  auto sp = std::get<Result<int>>(result).first;
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
  {
    auto so = std::make_shared<StateObject>();
    StateBehavior<Dummy, int, std::shared_ptr<Base>, std::shared_ptr<Base>> b{so};
    b.transition("", "state1", std::make_shared<Derived>(1, 2), std::make_shared<Derived>(2, 2));
    b.returns("state1", 1);
    // No polymorphism.
    auto result = b.call(std::make_shared<Derived>(10, 2), std::make_shared<Derived>(10, 2));
    DRTEST_ASSERT(std::holds_alternative<std::monostate>(result));
  }

  {
    auto so = std::make_shared<StateObject>();
    StateBehavior<Dummy, int, std::shared_ptr<Base>, std::shared_ptr<Base>> b{so};
    b.polymorphic<std::shared_ptr<Derived>, std::shared_ptr<Derived>>();
    b.transition("", "state1", std::make_shared<Derived>(1, 2), std::make_shared<Derived>(2, 2));
    b.returns("state1", 1);
    auto result = b.call(std::make_shared<Derived>(10, 2), std::make_shared<Derived>(10, 2));
    DRTEST_ASSERT(std::holds_alternative<Result<int>>(result));
    auto sp = std::get<Result<int>>(result).first;
    DRTEST_COMPARE(*sp, 1);
  }

  {
    auto so = std::make_shared<StateObject>();
    StateBehavior<Dummy, int, std::shared_ptr<Base>, std::shared_ptr<Base>> b{so};
    b.polymorphic<std::shared_ptr<Base>, std::shared_ptr<Derived>>();
    b.transition("", "state1", std::make_shared<Derived>(1, 2), std::make_shared<Derived>(2, 2));
    b.returns("state1", 1);
    auto result = b.call(std::make_shared<Derived>(1, 10), std::make_shared<Derived>(10, 2));
    DRTEST_ASSERT(std::holds_alternative<Result<int>>(result));
    auto sp = std::get<Result<int>>(result).first;
    DRTEST_COMPARE(*sp, 1);
  }

  {
    auto so = std::make_shared<StateObject>();
    StateBehavior<Dummy, int, std::shared_ptr<Base>, std::shared_ptr<Base>> b{so};
    b.polymorphic<std::shared_ptr<Derived>, std::shared_ptr<Base>>();
    b.transition("", "state1", std::make_shared<Derived>(1, 2), std::make_shared<Derived>(2, 2));
    b.returns("state1", 1);
    auto result = b.call(std::make_shared<Derived>(10, 2), std::make_shared<Derived>(2, 10));
    DRTEST_ASSERT(std::holds_alternative<Result<int>>(result));
    auto sp = std::get<Result<int>>(result).first;
    DRTEST_COMPARE(*sp, 1);
  }
}

DRTEST_TEST(wildcardOverride)
{
  auto so = std::make_shared<StateObject>();
  StateBehavior<Dummy, int, int> b{so};
  b.transition("*", "state2", 0);
  b.transition("", "state1", 0);
  b.returns("state1", 1);
  b.returns("state2", 2);

  auto result = b.call(0);
  DRTEST_ASSERT(std::holds_alternative<Result<int>>(result));
  auto cast = std::get<Result<int>>(result).first;
  DRTEST_COMPARE(*cast, 1);

  result = b.call(0);
  DRTEST_ASSERT(std::holds_alternative<Result<int>>(result));
  cast = std::get<Result<int>>(result).first;
  DRTEST_COMPARE(*cast, 2);
}

DRTEST_TEST(wildcardAsTargetState)
{
  auto so = std::make_shared<StateObject>();
  StateBehavior<Dummy, int, int> b{so};
  DRTEST_ASSERT_THROW(
      b.transition("", "*", 0),
      std::runtime_error
    );
}

DRTEST_TEST(wildcardResult)
{
  // Check that a wildcard result is used as catch-all.
  StateBehavior<Dummy, int> b{};
  b.transition("", "state1");
  b.transition("state1", "state2");
  b.transition("state2", "state3");
  b.returns("*", 999);
  b.returns("state2", 2);

  auto result = b.call();
  DRTEST_ASSERT(std::holds_alternative<Result<int>>(result));
  auto cast = std::get<Result<int>>(result).first;
  DRTEST_ASSERT_EQ(*cast, 999);

  result = b.call();
  DRTEST_ASSERT(std::holds_alternative<Result<int>>(result));
  cast = std::get<Result<int>>(result).first;
  DRTEST_ASSERT_EQ(*cast, 2);

  result = b.call();
  DRTEST_ASSERT(std::holds_alternative<Result<int>>(result));
  cast = std::get<Result<int>>(result).first;
  DRTEST_ASSERT_EQ(*cast, 999);
}

DRTEST_TEST(overridingPreviousResultWithThrow)
{
  // Test that using `returns` or `emits`, followed by throw raises an exception.
  StateBehavior<Dummy, int, int> b1{};
  b1.returns("state1", 1);
  DRTEST_ASSERT_THROW(
      b1.throws("state1", std::logic_error{""}),
      std::runtime_error
    );

  StateBehavior<Dummy, int, int> b2{};
  float f = 1.23f;
  double x = 1.23;
  b2.emits<int, float&, const double&>("state1", &Dummy::f, 123, f, x);
  DRTEST_ASSERT_THROW(
      b2.throws("state1", std::logic_error{""}),
      std::runtime_error
    );
}

DRTEST_TEST(overridingPreviousThrowWithResult)
{
  // Test that using `throw`, followed by `returns` or `emits` raises an exception.
  StateBehavior<Dummy, int, int> b1{};
  b1.throws("state1", std::logic_error{""});
  DRTEST_ASSERT_THROW(
      b1.returns("state1", 1),
      std::runtime_error
    );

  StateBehavior<Dummy, int, int> b2{};
  b2.throws("state1", std::logic_error{""});
  float f = 1.23f;
  double x = 1.23;
  DRTEST_ASSERT_THROW(
      (b2.emits<int, float&, const double&>("state1", &Dummy::f, 123, f, x)),
      std::runtime_error
    );
}

DRTEST_TEST(overridingReturnsAndEmits)
{
  // Double returns.
  StateBehavior<Dummy, int, int> b1{};
  b1.returns("state1", 1);
  DRTEST_ASSERT_THROW(
      b1.returns("state1", 2),
      std::runtime_error
    );

  // Double emits.
  StateBehavior<Dummy, int, int> b2{};
  float f = 1.23f;
  double x = 1.23;
  b2.emits<int, float&, const double&>("state1", &Dummy::f, 123, f, x);
  DRTEST_ASSERT_THROW(
      (b2.emits<int, float&, const double&>("state1", &Dummy::f, 123, f, x)),
      std::runtime_error
    );

  // Double returns, with emit.
  StateBehavior<Dummy, int, int> b3{};
  b3.emits<int, float&, const double&>("state1", &Dummy::f, 123, f, x);
  b3.returns("state1", 1);
  DRTEST_ASSERT_THROW(
      b3.returns("state1", 2),
      std::runtime_error
    );

  // Double emits, with returns.
  StateBehavior<Dummy, int, int> b4{};
  b4.returns("state1", 123);
  b4.emits<int, float&, const double&>("state1", &Dummy::f, 123, f, x);
  DRTEST_ASSERT_THROW(
      (b4.emits<int, float&, const double&>("state1", &Dummy::f, 123, f, x)),
      std::runtime_error
    );
}

DRTEST_TEST(overridingThrowsWithThrows)
{
  StateBehavior<Dummy, void> b{};
  b.throws("", std::logic_error{""});
  DRTEST_ASSERT_THROW(b.throws("", std::logic_error{""}), std::runtime_error);
}

DRTEST_TEST(convenienceTransitionPolymorphic)
{
  auto so = std::make_shared<StateObject>();
  StateBehavior<Dummy, int, std::shared_ptr<Base>, std::shared_ptr<Base>> b{so};

  // Use polymorphic first.
  b.polymorphic<std::shared_ptr<Base>, std::shared_ptr<Derived>>();
  b.transition("", "state1", std::make_shared<Derived>(1, 2), std::make_shared<Derived>(2, 2));
  b.returns("state1", 1);
  auto result = b.call(std::make_shared<Derived>(1, 10), std::make_shared<Derived>(10, 2));
  DRTEST_ASSERT(std::holds_alternative<Result<int>>(result));
  auto sp = std::get<Result<int>>(result).first;
  DRTEST_COMPARE(*sp, 1);

  // Check that the exception works.
  b.transition<std::shared_ptr<Base>, std::shared_ptr<Derived>>(
      "state1",
      "state2",
      std::make_shared<Derived>(3, 4),
      std::make_shared<Derived>(5, 6)
    );
  b.returns("state2", 2);
  result = b.call(std::make_shared<Derived>(3, 40), std::make_shared<Derived>(50, 6));
  DRTEST_ASSERT(std::holds_alternative<Result<int>>(result));
  sp = std::get<Result<int>>(result).first;
  DRTEST_COMPARE(*sp, 2);

  // Check that the default specified by polymorphic is still in use.
  b.transition("state2", "state3", std::make_shared<Derived>(1, 2), std::make_shared<Derived>(2, 2));
  b.returns("state3", 3);
  result = b.call(std::make_shared<Derived>(1, 10), std::make_shared<Derived>(10, 2));
  DRTEST_ASSERT(std::holds_alternative<Result<int>>(result));
  sp = std::get<Result<int>>(result).first;
  DRTEST_COMPARE(*sp, 3);
}
