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

#include <DrMock/test/Test.h>
#include <DrMock/mock/detail/IsEqual.h>

using namespace drmock;

class A
{
public:
  A(int x) : x_{x} {}
  virtual ~A() = default;
  bool operator==(const A& other) const { return x_ == other.x_; }
  int x() const { return x_; }
private:
  int x_;
};

class B : public A
{
public:
  B(int x, int y) : A{x}, y_{y} {}
  bool operator==(const B& other) const { return y_ == other.y_; }
  int y() const { return y_; }
private:
  int y_;
};

// Non-pointer, non-tuple types.
DRTEST_TEST(basic)
{
  auto is_equal = detail::IsEqual<A>{};

  DRTEST_ASSERT(is_equal(A{0}, A{0}));
  DRTEST_ASSERT(not is_equal(A{0}, A{1}));
}

DRTEST_TEST(shared)
{
  auto is_equal = detail::IsEqual<std::shared_ptr<A>>{};

  DRTEST_ASSERT(
      is_equal(std::make_shared<A>(0), std::make_shared<A>(0))
    );
  DRTEST_ASSERT(
      not is_equal(std::make_shared<A>(0), std::make_shared<A>(1))
    );
}

DRTEST_TEST(sharedConst)
{
  auto is_equal = detail::IsEqual<const std::shared_ptr<A>>{};

  DRTEST_ASSERT(
      is_equal(std::make_shared<A>(0), std::make_shared<A>(0))
    );
  DRTEST_ASSERT(
      not is_equal(std::make_shared<A>(0), std::make_shared<A>(1))
    );
}

DRTEST_TEST(sharedPolymorphic)
{
  auto is_equal = detail::IsEqual<std::shared_ptr<A>, std::shared_ptr<B>>{};

  // Equal objects of type B.
  DRTEST_ASSERT(
      is_equal(std::make_shared<B>(0, 0), std::make_shared<B>(0, 0))
    );

  // Non-equal objects of type B.
  DRTEST_ASSERT(
      not is_equal(std::make_shared<B>(0, 0), std::make_shared<B>(0, 1))
    );

  // Dynamic pointer cast failures.
  DRTEST_ASSERT(
      not is_equal(std::make_shared<A>(0), std::make_shared<B>(0, 1))
    );
  DRTEST_ASSERT(
      not is_equal(std::make_shared<B>(0, 1), std::make_shared<A>(0))
    );
  DRTEST_ASSERT(
      not is_equal(std::make_shared<A>(0), std::make_shared<A>(0))
    );
}

DRTEST_TEST(unique)
{
  auto is_equal = detail::IsEqual<std::unique_ptr<A>>{};

  DRTEST_ASSERT(
      is_equal(std::make_unique<A>(0), std::make_unique<A>(0))
    );
  DRTEST_ASSERT(
      not is_equal(std::make_unique<A>(0), std::make_unique<A>(1))
    );
}

DRTEST_TEST(uniqueConst)
{
  auto is_equal = detail::IsEqual<const std::unique_ptr<A>>{};

  DRTEST_ASSERT(
      is_equal(std::make_unique<A>(0), std::make_unique<A>(0))
    );
  DRTEST_ASSERT(
      not is_equal(std::make_unique<A>(0), std::make_unique<A>(1))
    );
}

DRTEST_TEST(uniquePolymorphic)
{
  auto is_equal = detail::IsEqual<std::unique_ptr<A>, std::unique_ptr<B>>{};

  // Equal objects of type B.
  DRTEST_ASSERT(
      is_equal(std::make_unique<B>(0, 0), std::make_unique<B>(0, 0))
    );

  // Non-equal objects of type B.
  DRTEST_ASSERT(
      not is_equal(std::make_unique<B>(0, 0), std::make_unique<B>(0, 1))
    );

  // Dynamic pointer cast failures.
  DRTEST_ASSERT(
      not is_equal(std::make_unique<A>(0), std::make_unique<B>(0, 1))
    );
  DRTEST_ASSERT(
      not is_equal(std::make_unique<B>(0, 1), std::make_unique<A>(0))
    );
  DRTEST_ASSERT(
      not is_equal(std::make_unique<A>(0), std::make_unique<A>(0))
    );
}

DRTEST_TEST(tupleBasic)
{
  auto is_equal = detail::IsEqual<std::tuple<A, A, A>>{};

  // First, second and thrid components are equal.
  auto x = std::make_tuple(A{0}, A{1}, A{2});
  auto y = std::make_tuple(A{0}, A{1}, A{2});
  DRTEST_ASSERT(is_equal(x, y));

  // First components are not equal.
  x = std::make_tuple(A{0}, A{1}, A{2});
  y = std::make_tuple(A{-1}, A{1}, A{2});
  DRTEST_ASSERT(not is_equal(x, y));

  // Second components are not equal.
  x = std::make_tuple(A{0}, A{1}, A{2});
  y = std::make_tuple(A{0}, A{-1}, A{2});
  DRTEST_ASSERT(not is_equal(x, y));

  // Third components are not equal.
  x = std::make_tuple(A{0}, A{1}, A{2});
  y = std::make_tuple(A{0}, A{-1}, A{-2});
  DRTEST_ASSERT(not is_equal(x, y));
}

DRTEST_TEST(tupleSharedPolymorphic)
{
  auto is_equal = detail::IsEqual<
      std::tuple<std::shared_ptr<A>, std::shared_ptr<A>, std::shared_ptr<A>>,
      std::tuple<std::shared_ptr<B>, std::shared_ptr<B>, std::shared_ptr<B>>
    >{};
  std::tuple<std::shared_ptr<A>, std::shared_ptr<A>, std::shared_ptr<A>> x;
  std::tuple<std::shared_ptr<A>, std::shared_ptr<A>, std::shared_ptr<A>> y;

  // First, second, third components are all equal.
  x = std::make_tuple(
      std::make_shared<B>(0, 1),
      std::make_shared<B>(2, 3),
      std::make_shared<B>(4, 5)
    );
  y = std::make_tuple(
      std::make_shared<B>(0, 1),
      std::make_shared<B>(2, 3),
      std::make_shared<B>(4, 5)
    );
  DRTEST_ASSERT(is_equal(x, y));

  // First components are not equal.

  x = std::make_tuple(
      std::make_shared<B>(0, 1),
      std::make_shared<B>(2, 3),
      std::make_shared<B>(4, 5)
    );
  y = std::make_tuple(
      std::make_shared<B>(0, -1),
      std::make_shared<B>(2, 3),
      std::make_shared<B>(4, 5)
    );
  DRTEST_ASSERT(not is_equal(x, y));


  // First component fails dynamic cast.
  x = std::make_tuple(
      std::make_shared<B>(0, 1),
      std::make_shared<B>(2, 3),
      std::make_shared<B>(4, 5)
    );
  y = std::make_tuple(
      std::make_shared<A>(0),
      std::make_shared<B>(2, 3),
      std::make_shared<B>(4, 5)
    );
  DRTEST_ASSERT(not is_equal(x, y));


  // Second components are not equal.
  x = std::make_tuple(
      std::make_shared<B>(0, 1),
      std::make_shared<B>(2, 3),
      std::make_shared<B>(4, 5)
    );
  y = std::make_tuple(
      std::make_shared<B>(0, 1),
      std::make_shared<B>(2, 234),
      std::make_shared<B>(4, 5)
    );
  DRTEST_ASSERT(not is_equal(x, y));

  // Second component fails dynamic cast.
  x = std::make_tuple(
      std::make_shared<B>(0, 1),
      std::make_shared<B>(2, 3),
      std::make_shared<B>(4, 5)
    );
  y = std::make_tuple(
      std::make_shared<B>(0, 1),
      std::make_shared<A>(2),
      std::make_shared<B>(4, 5)
    );
  DRTEST_ASSERT(not is_equal(x, y));

  // Third components are not equal.
  x = std::make_tuple(
      std::make_shared<B>(0, 1),
      std::make_shared<B>(2, 3),
      std::make_shared<B>(4, 5)
    );
  y = std::make_tuple(
      std::make_shared<B>(0, 1),
      std::make_shared<B>(2, 3),
      std::make_shared<B>(4, 567)
    );
  DRTEST_ASSERT(not is_equal(x, y));

  // Third component fails dynamic cast.
  x = std::make_tuple(
      std::make_shared<B>(0, 1),
      std::make_shared<B>(2, 3),
      std::make_shared<B>(4, 5)
    );
  y = std::make_tuple(
      std::make_shared<B>(0, 1),
      std::make_shared<B>(2, 3),
      std::make_shared<A>(4)
    );
  DRTEST_ASSERT(not is_equal(x, y));
}

DRTEST_TEST(tupleRecursion)
{
  auto is_equal = detail::IsEqual<
      std::tuple<int, std::tuple<int, std::tuple<float>>, double>
    >{};

  // Middle tuple is equal.
  auto x = std::make_tuple(1, std::make_tuple(2, std::make_tuple(3.4f)), 5.6);
  auto y = std::make_tuple(1, std::make_tuple(2, std::make_tuple(3.4f)), 5.6);
  DRTEST_ASSERT(is_equal(x, y));

  // Middle tuple is not equal.
  x = std::make_tuple(1, std::make_tuple(2, std::make_tuple(3.4f)), 5.6);
  y = std::make_tuple(1, std::make_tuple(3, std::make_tuple(3.4f)), 5.6);
  DRTEST_ASSERT(not is_equal(x, y));
  x = std::make_tuple(1, std::make_tuple(2, std::make_tuple(3.4f)), 5.6);
  y = std::make_tuple(1, std::make_tuple(2, std::make_tuple(4.3f)), 5.6);
  DRTEST_ASSERT(not is_equal(x, y));
}

DRTEST_TEST(rawPointer)
{
  auto is_equal = detail::IsEqual<int*, int*>{};
  int* x1 = new int{123};
  int* x2 = new int{123};
  DRTEST_ASSERT(is_equal(x1, x2));
  delete x1;
  delete x2;
}

DRTEST_TEST(rawPointerConst)
{
  auto is_equal = detail::IsEqual<int* const, int* const>{};
  int* const x1 = new int{123};
  int* const x2 = new int{123};
  DRTEST_ASSERT(is_equal(x1, x2));
  delete x1;
  delete x2;
}

DRTEST_TEST(rawPointerToConst)
{
  auto is_equal = detail::IsEqual<int const*, int const*>{};
  int const* x1 = new int{123};
  int const* x2 = new int{123};
  DRTEST_ASSERT(is_equal(x1, x2));
  delete x1;
  delete x2;
}

DRTEST_TEST(rawPointerToPointer)
{
  auto is_equal = detail::IsEqual<int const**, int const**>{};
  int const** x1 = new int const*{new int {123}};
  int const** x2 = new int const*{new int {123}}; // p -> const p -> const i
  DRTEST_ASSERT(is_equal(x1, x2));
  delete *x1;
  delete *x2;
  delete x1;
  delete x2;
}

DRTEST_TEST(rawPointerToConstPointer)
{
  auto is_equal = detail::IsEqual<int const* const*, int const* const*>{};
  int const* const* x1 = new int const*{new int {123}};
  int const* const* x2 = new int const*{new int {123}}; // p -> const p -> const i
  DRTEST_ASSERT(is_equal(x1, x2));
  delete *x1;
  delete *x2;
  delete x1;
  delete x2;
}

DRTEST_TEST(derivedForBase)
{
  auto is_equal = detail::IsEqual<std::shared_ptr<A>>{};
  auto p1 = std::make_shared<B>(1, 2);
  auto p2 = std::make_shared<B>(3, 4);
  DRTEST_ASSERT(not is_equal(p1, p2));
}
