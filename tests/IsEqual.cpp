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

#include "mock/detail/IsEqual.h"

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

// Non-pointer and non-tuple types
DRTEST_TEST(basic)
{
  auto is_equal = detail::IsEqual<A>{};
  DRTEST_COMPARE(
      is_equal(A{0}, A{0}),
      true
    );
  DRTEST_COMPARE(
      is_equal(A{0}, A{1}), 
      false
    );
}

DRTEST_TEST(shared)
{
  auto is_equal = detail::IsEqual<std::shared_ptr<A>>{};
  DRTEST_COMPARE(
      is_equal(std::make_shared<A>(0), std::make_shared<A>(0)),
      true
    );
  DRTEST_COMPARE(
      is_equal(std::make_shared<A>(0), std::make_shared<A>(1)),
      false
    );
}

DRTEST_TEST(sharedConst)
{
  auto is_equal = detail::IsEqual<const std::shared_ptr<A>>{};
  DRTEST_COMPARE(
      is_equal(std::make_shared<A>(0), std::make_shared<A>(0)),
      true
    );
  DRTEST_COMPARE(
      is_equal(std::make_shared<A>(0), std::make_shared<A>(1)),
      false
    );
}

DRTEST_TEST(polymorphic_shared)
{
  auto is_equal = detail::IsEqual<std::shared_ptr<A>, std::shared_ptr<B>>{};
  // Equal objects of type B
  DRTEST_COMPARE(
      is_equal(std::make_shared<B>(0, 0), std::make_shared<B>(0, 0)),
      true
    );
  // Non-equal objects of type B
  DRTEST_COMPARE(
      is_equal(std::make_shared<B>(0, 0), std::make_shared<B>(0, 1)),
      false
    );
  // Dynamic pointer cast failures
  DRTEST_COMPARE(
      is_equal(std::make_shared<A>(0), std::make_shared<B>(0, 1)),
      false
    );
  DRTEST_COMPARE(
      is_equal(std::make_shared<B>(0, 1), std::make_shared<A>(0)),
      false
    );
  DRTEST_COMPARE(
      is_equal(std::make_shared<A>(0), std::make_shared<A>(0)),
      false
    );
}

DRTEST_TEST(unique)
{
  auto is_equal = detail::IsEqual<std::unique_ptr<A>>{};
  DRTEST_COMPARE(
      is_equal(std::make_unique<A>(0), std::make_unique<A>(0)),
      true
    );
  DRTEST_COMPARE(
      is_equal(std::make_unique<A>(0), std::make_unique<A>(1)),
      false
    );
}

DRTEST_TEST(uniqueConst)
{
  auto is_equal = detail::IsEqual<const std::unique_ptr<A>>{};
  DRTEST_COMPARE(
      is_equal(std::make_unique<A>(0), std::make_unique<A>(0)),
      true
    );
  DRTEST_COMPARE(
      is_equal(std::make_unique<A>(0), std::make_unique<A>(1)),
      false
    );
}

DRTEST_TEST(polymorphic_unique)
{
  auto is_equal = detail::IsEqual<std::unique_ptr<A>, std::unique_ptr<B>>{};
  // Equal objects of type B
  DRTEST_COMPARE(
      is_equal(std::make_unique<B>(0, 0), std::make_unique<B>(0, 0)),
      true
    );
  // Non-equal objects of type B
  DRTEST_COMPARE(
      is_equal(std::make_unique<B>(0, 0), std::make_unique<B>(0, 1)),
      false
    );
  // Dynamic pointer cast failures
  DRTEST_COMPARE(
      is_equal(std::make_unique<A>(0), std::make_unique<B>(0, 1)),
      false
    );
  DRTEST_COMPARE(
      is_equal(std::make_unique<B>(0, 1), std::make_unique<A>(0)),
      false
    );
  DRTEST_COMPARE(
      is_equal(std::make_unique<A>(0), std::make_unique<A>(0)),
      false
    );
}

DRTEST_TEST(tuple_basic)
{
  auto is_equal = detail::IsEqual<std::tuple<A, A, A>>{};
  // First, second and thrid components are equal.
  DRTEST_COMPARE(
      is_equal(
          std::make_tuple(A{0}, A{1}, A{2}), 
          std::make_tuple(A{0}, A{1}, A{2})
        ),
      true
    );
  // First components are not equal.
  DRTEST_COMPARE(
      is_equal(
          std::make_tuple(A{0}, A{1}, A{2}), 
          std::make_tuple(A{-1}, A{1}, A{2})
        ),
      false
    );
  // Second components are not equal.
  DRTEST_COMPARE(
      is_equal(
          std::make_tuple(A{0}, A{1}, A{2}), 
          std::make_tuple(A{0}, A{-1}, A{2})
        ),
      false
    );
  // Third components are not equal.
  DRTEST_COMPARE(
      is_equal(
          std::make_tuple(A{0}, A{1}, A{2}), 
          std::make_tuple(A{0}, A{1}, A{-1})
        ),
      false
    );
}

DRTEST_TEST(tuple_shared)
{
  auto is_equal = detail::IsEqual<
      std::tuple<
          std::shared_ptr<A>, 
          std::shared_ptr<A>, 
          std::shared_ptr<A>
        >
    >{};
  // First, second and third components are equal.
  DRTEST_COMPARE(
      is_equal(
          std::make_tuple(
              std::make_shared<A>(0), 
              std::make_shared<A>(1), 
              std::make_shared<A>(2)
            ),
          std::make_tuple(
              std::make_shared<A>(0), 
              std::make_shared<A>(1), 
              std::make_shared<A>(2)
            )
        ),
      true
    );
  // First components are not equal.
  DRTEST_COMPARE(
      is_equal(
          std::make_tuple(
              std::make_shared<A>(0), 
              std::make_shared<A>(1), 
              std::make_shared<A>(2)
            ),
          std::make_tuple(
              std::make_shared<A>(-1), 
              std::make_shared<A>(1), 
              std::make_shared<A>(2)
            )
        ),
      false
    );
  // Second components are not equal.
  DRTEST_COMPARE(
      is_equal(
          std::make_tuple(
              std::make_shared<A>(0), 
              std::make_shared<A>(1), 
              std::make_shared<A>(2)
            ),
          std::make_tuple(
              std::make_shared<A>(0), 
              std::make_shared<A>(-1), 
              std::make_shared<A>(2)
            )
        ),
      false
    );
  // Third components are not equal.
  DRTEST_COMPARE(
      is_equal(
          std::make_tuple(
              std::make_shared<A>(0), 
              std::make_shared<A>(1), 
              std::make_shared<A>(2)
            ),
          std::make_tuple(
              std::make_shared<A>(0), 
              std::make_shared<A>(1), 
              std::make_shared<A>(-1)
            )
        ),
      false
    );
}

//  TODO Remaining cases: (1) Combining different types, such as
// 
//  IsEqual<
//      std::tuple<
//          std::shared_ptr<A>,
//          std::shared_ptr<T>
//        >,
//      std::tuple<
//          std::shared_ptr<B>,
//          std::sharedyptr<U>
//        >
//    >;
//
//  (2) Symmetries, i.e. "lhs fails dynamic cast" is tested, but "rhs fails
//    dynamic cast" is not.
//
//  (3) Tuple to tuple recursion
//
//  Mock each specialization of IsEqual<...> individually?

DRTEST_TEST(tuple_polymorphic)
{
  auto is_equal = detail::IsEqual<
      std::tuple<
          std::shared_ptr<A>, 
          std::shared_ptr<A>, 
          std::shared_ptr<A>
        >,
      std::tuple<
          std::shared_ptr<B>,
          std::shared_ptr<B>, 
          std::shared_ptr<B>
        >
    >{};
  // First, second, third components are all equal
  DRTEST_COMPARE(
      is_equal(
          std::make_tuple(
              std::make_shared<B>(0, 1),
              std::make_shared<B>(2, 3), 
              std::make_shared<B>(4, 5)
            ),
          std::make_tuple(
              std::make_shared<B>(0, 1),
              std::make_shared<B>(2, 3), 
              std::make_shared<B>(4, 5)
            )
        ),
      true
    );
  // First components are not equal
  DRTEST_COMPARE(
      is_equal(
          std::make_tuple(
              std::make_shared<B>(0, 1),
              std::make_shared<B>(2, 3), 
              std::make_shared<B>(4, 5)
            ),
          std::make_tuple(
              std::make_shared<B>(0, -1),
              std::make_shared<B>(2, 3), 
              std::make_shared<B>(4, 5)
            )
        ),
      false
    );
  // First component fails dynamic cast
  DRTEST_COMPARE(
      is_equal(
          std::make_tuple(
              std::make_shared<B>(0, 1),
              std::make_shared<B>(2, 3), 
              std::make_shared<B>(4, 5)
            ),
          std::make_tuple(
              std::make_shared<A>(0),
              std::make_shared<B>(2, 3), 
              std::make_shared<B>(4, 5)
            )
        ),
      false
    );
  // Second components are not equal
  DRTEST_COMPARE(
      is_equal(
          std::make_tuple(
              std::make_shared<B>(0, 1),
              std::make_shared<B>(2, 3), 
              std::make_shared<B>(4, 5)
            ),
          std::make_tuple(
              std::make_shared<B>(0, 1),
              std::make_shared<B>(2, -1), 
              std::make_shared<B>(4, 5)
            )
        ),
      false
    );
  // Second component fails dynamic cast
  DRTEST_COMPARE(
      is_equal(
          std::make_tuple(
              std::make_shared<B>(0, 1),
              std::make_shared<B>(2, 3), 
              std::make_shared<B>(4, 5)
            ),
          std::make_tuple(
              std::make_shared<B>(0, 1), 
              std::make_shared<A>(2),
              std::make_shared<B>(4, 5)
            )
        ),
      false
    );
  // Third components are not equal
  DRTEST_COMPARE(
      is_equal(
          std::make_tuple(
              std::make_shared<B>(0, 1),
              std::make_shared<B>(2, 3), 
              std::make_shared<B>(4, 5)
            ),
          std::make_tuple(
              std::make_shared<B>(0, 1),
              std::make_shared<B>(2, 3), 
              std::make_shared<B>(4, -1)
            )
        ),
      false
    );
  // Third component fails dynamic cast
  DRTEST_COMPARE(
      is_equal(
          std::make_tuple(
              std::make_shared<B>(0, 1),
              std::make_shared<B>(2, 3), 
              std::make_shared<B>(4, 5)
            ),
          std::make_tuple(
              std::make_shared<B>(0, 1), 
              std::make_shared<B>(2, 3),
              std::make_shared<A>(4)
            )
        ),
      false
    );
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
