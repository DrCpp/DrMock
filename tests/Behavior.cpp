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
#include "mock/Behavior.h"

using namespace drmock;

class Dummy
{
public:
  void f(int, float, double) {};
};

template<typename ReturnType>
using Result = std::pair<
    std::shared_ptr<ReturnType>,
    std::shared_ptr<AbstractSignal<Dummy>>
  >;

DRTEST_TEST(exhausted)
{
  Behavior<Dummy, void, int, std::string> b{};

  // Per default, one call is expected for exhaustion.
  DRTEST_ASSERT(not b.is_exhausted());

  b.expects(2, "foo").times(2);
  DRTEST_ASSERT(not b.is_exhausted());

  b.produce();
  DRTEST_ASSERT(not b.is_exhausted());

  b.produce();
  DRTEST_ASSERT(b.is_exhausted());

  // Excess calls don't change exhaustion.
  b.produce();
  DRTEST_ASSERT(b.is_exhausted());
}

DRTEST_TEST(exhaustedMinMax)
{
  Behavior<Dummy, void, int, std::string> b{};

  b.expects(2, "foo").times(2, 4);
  DRTEST_ASSERT(not b.is_exhausted());

  b.produce();
  DRTEST_ASSERT(not b.is_exhausted());

  b.produce();
  DRTEST_ASSERT(b.is_exhausted());

  b.produce();
  DRTEST_ASSERT(b.is_exhausted());

  // Excess calls don't change exhaustion.
  b.produce();
  DRTEST_ASSERT(b.is_exhausted());
}

DRTEST_TEST(persistentImpliesExhausted)
{
  Behavior<Dummy, void, int, std::string> b{};
  b.persists();
  DRTEST_ASSERT(b.is_exhausted());
}

DRTEST_TEST(voidReturn)
{
  Behavior<Dummy, void, int, std::string> b{};
  b.expects(2, "foo");
  DRTEST_ASSERT(not b.match(2, "foobar"));
  DRTEST_ASSERT(b.match(2, "foo"));
  DRTEST_ASSERT(b.is_persistent());

  auto result = b.produce();
  auto holds = std::holds_alternative<Result<void>>(result);
  DRTEST_ASSERT(holds);
  DRTEST_ASSERT(not std::get<Result<void>>(result).first);
  DRTEST_ASSERT(not std::get<Result<void>>(result).second);
  DRTEST_ASSERT(not b.is_persistent());

  b.persists();
  result = b.produce();
  result = b.produce();
  result = b.produce();
  DRTEST_ASSERT(b.is_persistent());

  b.emits(&Dummy::f, 1, 2.3f, 4.56);
  result = b.produce();
  DRTEST_ASSERT(std::holds_alternative<Result<void>>(result));
  DRTEST_ASSERT(not std::get<Result<void>>(result).first);
  DRTEST_ASSERT(std::get<Result<void>>(result).second);

  b.throws(std::runtime_error{""});
  result = b.produce();
  DRTEST_ASSERT(std::holds_alternative<std::exception_ptr>(result));
  DRTEST_ASSERT_THROW(
      std::rethrow_exception(std::get<std::exception_ptr>(result)),
      std::runtime_error
    );
}

DRTEST_TEST(matchAllExplicit)
{
  Behavior<Dummy, void, int, std::string> b{};
  b.expects();
  DRTEST_ASSERT(b.match(123, "foo"));
  DRTEST_ASSERT(b.match(456, "bar"));
}

DRTEST_TEST(matchAllImplicit)
{
  Behavior<Dummy, void, int, std::string> b{};
  DRTEST_ASSERT(b.match(123, "foo"));
  DRTEST_ASSERT(b.match(456, "bar"));
}

DRTEST_TEST(nonVoidMethod)
{
  Behavior<Dummy, int&, int, std::string> b{};
  b.expects(2, "foo")
   .returns(12);
  DRTEST_ASSERT(not b.match(2, "foobar"));
  DRTEST_ASSERT(b.match(2, "foo"));
  DRTEST_ASSERT(b.is_persistent());

  auto result = b.produce();
  DRTEST_ASSERT(std::holds_alternative<Result<int>>(result));
  auto ptr = std::get<Result<int>>(result).first;
  DRTEST_COMPARE(*ptr, 12);
  DRTEST_ASSERT(not b.is_persistent());

  b.persists();
  result = b.produce();
  result = b.produce();
  result = b.produce();
  DRTEST_ASSERT(b.is_persistent());

  b.emits(&Dummy::f, 1, 2.3f, 4.56);
  result = b.produce();
  DRTEST_ASSERT(std::holds_alternative<Result<int>>(result));
  ptr = std::get<Result<int>>(result).first;
  DRTEST_ASSERT_EQ(*ptr, 12);
  DRTEST_ASSERT(std::get<Result<int>>(result).second);

  b.throws(std::runtime_error{""});
  result = b.produce();
  DRTEST_ASSERT(std::holds_alternative<std::exception_ptr>(result));
  DRTEST_ASSERT_THROW(
      std::rethrow_exception(std::get<std::exception_ptr>(result)),
      std::runtime_error
    );
}

DRTEST_TEST(voidVoidMatchExplicit)
{
  Behavior<Dummy, void> b{};
  b.expects();
  DRTEST_ASSERT(b.match());
}

DRTEST_TEST(voidVoidMatchImplicit)
{
  Behavior<Dummy, void> b{};
  DRTEST_ASSERT(b.match());
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

DRTEST_TEST(pointerBase)
{
  Behavior<Dummy, void, std::shared_ptr<Base>, std::shared_ptr<Base>> b{};
  b.expects(std::make_shared<Base>(1), std::make_shared<Base>(2));
  DRTEST_ASSERT(
      not b.match(std::make_shared<Base>(2), std::make_shared<Base>(1))
    );
  DRTEST_ASSERT(
      b.match(std::make_shared<Base>(1), std::make_shared<Base>(2))
    );
}

DRTEST_TEST(pointerDerived)
{
  Behavior<Dummy, void, std::shared_ptr<Derived>, std::shared_ptr<Derived>> b{};
  b.expects(std::make_shared<Derived>(1, 1), std::make_shared<Derived>(1, 2));
  DRTEST_ASSERT(
      not b.match(std::make_shared<Derived>(1, 2), std::make_shared<Derived>(1, 1))
    );
  DRTEST_ASSERT(
      b.match(std::make_shared<Derived>(5, 1), std::make_shared<Derived>(6, 2))
    );
}

DRTEST_TEST(polymorphic)
{
  Behavior<Dummy, void, std::shared_ptr<Base>, std::shared_ptr<Base>> b{};
  b.expects(std::make_shared<Derived>(1, 2), std::make_shared<Derived>(2, 2));
  DRTEST_ASSERT(
      not b.match(std::make_shared<Derived>(10, 2), std::make_shared<Derived>(10, 2))
    ); // does not match because the base type is compared and the x don't match

  b.polymorphic<std::shared_ptr<Derived>, std::shared_ptr<Derived>>();
  DRTEST_ASSERT(
      b.match(std::make_shared<Derived>(10, 2), std::make_shared<Derived>(10, 2))
    );

  b.polymorphic<std::shared_ptr<Base>, std::shared_ptr<Derived>>();
  DRTEST_ASSERT(
      b.match(std::make_shared<Derived>(1, 10), std::make_shared<Derived>(10, 2))
    );

  b.polymorphic<std::shared_ptr<Derived>, std::shared_ptr<Base>>();
  DRTEST_ASSERT(
      b.match(std::make_shared<Derived>(10, 2), std::make_shared<Derived>(2, 10))
    );
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
  Behavior<Dummy, void, std::shared_ptr<Interface>, std::shared_ptr<Interface>> b{
      std::make_shared<detail::IsTuplePackEqual<
            std::tuple<std::shared_ptr<Interface>, std::shared_ptr<Interface>>,
            std::tuple<std::shared_ptr<Implementation>, std::shared_ptr<Implementation>>
          >>()
    };
  b.expects(std::make_shared<Implementation>(1), std::make_shared<Implementation>(2));
  DRTEST_ASSERT(
      not b.match(std::make_shared<Implementation>(2), std::make_shared<Implementation>(1))
    );
  DRTEST_ASSERT(
      b.match(std::make_shared<Implementation>(1), std::make_shared<Implementation>(2))
    );
}

DRTEST_TEST(polymorphicPureVirtualDefaultConstructor)
{
  Behavior<Dummy, void, std::shared_ptr<Interface>, std::shared_ptr<Interface>> b{};
  b.expects(std::make_shared<Implementation>(1), std::make_shared<Implementation>(2));
  DRTEST_ASSERT_THROW(
      b.match(std::make_shared<Implementation>(1), std::make_shared<Implementation>(2)),
      std::logic_error
    );

  b.polymorphic<std::shared_ptr<Implementation>, std::shared_ptr<Implementation>>();
  DRTEST_ASSERT(
      not b.match(std::make_shared<Implementation>(2), std::make_shared<Implementation>(1))
    );
  DRTEST_ASSERT(
      b.match(std::make_shared<Implementation>(1), std::make_shared<Implementation>(2))
    );
}
