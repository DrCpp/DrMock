/* Copyright 2021 Ole Kliemann, Malte Kliemann
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

#include "mock/detail/Invoke.h"

using namespace drmock;

class A
{
public:
  A() = default;
  A(int x) : x_{x} {}
  virtual ~A() = default;
  bool operator==(const A& other) const { return x_ == other.x_; }
  int x() const { return x_; }
private:
  int x_{0};
};

class B : public A
{
public:
  B() = default;
  B(int x, int y) : A{x}, y_{y} {}
  bool operator==(const B& other) const { return y_ == other.y_; }
  int y() const { return y_; }
private:
  int y_{0};
};

template<typename T>
class Invocable : public IInvocable<T>
{
public:
  Invocable(bool result) : result_{result}, t_{} {}
  bool invoke(const T& t) const
  {
    called_ = true;
    t_ = t;
    return result_;
  }
  T t() const { return t_; }
  bool called() const { return called_; }

private:
  mutable bool called_ = false;
  bool result_;
  mutable T t_;
};

// Non-tuple types.
DRTEST_DATA(basic)
{
  drtest::addColumn<bool>("result");
  drtest::addRow("true", true);
  drtest::addRow("false", false);
}

DRTEST_TEST(basic)
{
  DRTEST_FETCH(bool, result);
  auto value = A{7};

  auto invoke = detail::Invoke<std::shared_ptr<IInvocable<A>>, A>{};
  auto invocable = std::make_shared<Invocable<A>>(result);
  DRTEST_ASSERT_EQ(invoke(invocable, value), result);
  DRTEST_ASSERT_EQ(invocable->t(), value);
}

DRTEST_DATA(flatTuple)
{
  drtest::addColumn<bool>("result1");
  drtest::addColumn<bool>("result2");
  drtest::addRow("tt", true, true);
  drtest::addRow("tf", true, false);
  drtest::addRow("ft", false, true);
  drtest::addRow("ff", false, false);
}

DRTEST_TEST(flatTuple)
{
  DRTEST_FETCH(bool, result1);
  DRTEST_FETCH(bool, result2);
  auto value = std::make_tuple(A{1}, A{2});

  auto invocable = std::make_tuple(
      std::make_shared<Invocable<A>>(result1),
      std::make_shared<Invocable<A>>(result2)
    );
  DRTEST_ASSERT_EQ(std::get<0>(invocable)->t().x(), A{0}.x());
  DRTEST_ASSERT_EQ(std::get<1>(invocable)->t().x(), A{0}.x());
  auto invoke = detail::Invoke<
      std::tuple<
          std::shared_ptr<IInvocable<A>>,
          std::shared_ptr<IInvocable<A>>
        >,
      std::tuple<A, A>
    >{};
  DRTEST_ASSERT_EQ(invoke(invocable, value), result1*result2);
  DRTEST_ASSERT_EQ(std::get<0>(invocable)->t(), A{1});
  // Can't test this, due to short-circuit evaluation of fold expression
  // in Invoke.
  // DRTEST_ASSERT(std::get<1>(invocable)->called());
  // DRTEST_ASSERT_EQ(std::get<1>(invocable)->t().x(), A{2}.x());
}

// DRTEST_DATA(recurseTuple)
// {
//   drtest::addColumn<bool>("result");
//   drtest::addColumn<std::tuple<int, std::tuple<float, std::string>>>("value");
// 
//   drtest::addRow("true", true, std::make_shared<A>(4));
//   drtest::addRow("false", false, std::make_shared<A>(5));
// }
// 
// DRTEST_TEST(recurseTuple)
// {
//   DRTEST_FETCH(bool, result);
//   DRTEST_FETCH(std::shared_ptr<A>, value);
// 
//   auto invoke = detail::Invoke<std::shared_ptr<A>>{};
//   auto invocable = std::make_shared<Invocable<std::shared_ptr<A>>>(result);
//   DRTEST_ASSERT_EQ(invoke(invocable, value), result);
//   DRTEST_ASSERT_EQ(*invocable->t(), *value);
// }

// 
// DRTEST_TEST(sharedConst)
// {
//   auto is_equal = detail::IsEqual<const std::shared_ptr<A>>{};
// 
//   DRTEST_ASSERT(
//       is_equal(std::make_shared<A>(0), std::make_shared<A>(0))
//     );
//   DRTEST_ASSERT(
//       not is_equal(std::make_shared<A>(0), std::make_shared<A>(1))
//     );
// }
// 
// DRTEST_TEST(sharedPolymorphic)
// {
//   auto is_equal = detail::IsEqual<std::shared_ptr<A>, std::shared_ptr<B>>{};
// 
//   // Equal objects of type B.
//   DRTEST_ASSERT(
//       is_equal(std::make_shared<B>(0, 0), std::make_shared<B>(0, 0))
//     );
// 
//   // Non-equal objects of type B.
//   DRTEST_ASSERT(
//       not is_equal(std::make_shared<B>(0, 0), std::make_shared<B>(0, 1))
//     );
// 
//   // Dynamic pointer cast failures.
//   DRTEST_ASSERT(
//       not is_equal(std::make_shared<A>(0), std::make_shared<B>(0, 1))
//     );
//   DRTEST_ASSERT(
//       not is_equal(std::make_shared<B>(0, 1), std::make_shared<A>(0))
//     );
//   DRTEST_ASSERT(
//       not is_equal(std::make_shared<A>(0), std::make_shared<A>(0))
//     );
// }
// 
// DRTEST_TEST(unique)
// {
//   auto is_equal = detail::IsEqual<std::unique_ptr<A>>{};
// 
//   DRTEST_ASSERT(
//       is_equal(std::make_unique<A>(0), std::make_unique<A>(0))
//     );
//   DRTEST_ASSERT(
//       not is_equal(std::make_unique<A>(0), std::make_unique<A>(1))
//     );
// }
// 
// DRTEST_TEST(uniqueConst)
// {
//   auto is_equal = detail::IsEqual<const std::unique_ptr<A>>{};
// 
//   DRTEST_ASSERT(
//       is_equal(std::make_unique<A>(0), std::make_unique<A>(0))
//     );
//   DRTEST_ASSERT(
//       not is_equal(std::make_unique<A>(0), std::make_unique<A>(1))
//     );
// }
// 
// DRTEST_TEST(uniquePolymorphic)
// {
//   auto is_equal = detail::IsEqual<std::unique_ptr<A>, std::unique_ptr<B>>{};
// 
//   // Equal objects of type B.
//   DRTEST_ASSERT(
//       is_equal(std::make_unique<B>(0, 0), std::make_unique<B>(0, 0))
//     );
// 
//   // Non-equal objects of type B.
//   DRTEST_ASSERT(
//       not is_equal(std::make_unique<B>(0, 0), std::make_unique<B>(0, 1))
//     );
// 
//   // Dynamic pointer cast failures.
//   DRTEST_ASSERT(
//       not is_equal(std::make_unique<A>(0), std::make_unique<B>(0, 1))
//     );
//   DRTEST_ASSERT(
//       not is_equal(std::make_unique<B>(0, 1), std::make_unique<A>(0))
//     );
//   DRTEST_ASSERT(
//       not is_equal(std::make_unique<A>(0), std::make_unique<A>(0))
//     );
// }
// 
// DRTEST_TEST(tupleBasic)
// {
//   auto is_equal = detail::IsEqual<std::tuple<A, A, A>>{};
// 
//   // First, second and thrid components are equal.
//   auto x = std::make_tuple(A{0}, A{1}, A{2});
//   auto y = std::make_tuple(A{0}, A{1}, A{2});
//   DRTEST_ASSERT(is_equal(x, y));
// 
//   // First components are not equal.
//   x = std::make_tuple(A{0}, A{1}, A{2});
//   y = std::make_tuple(A{-1}, A{1}, A{2});
//   DRTEST_ASSERT(not is_equal(x, y));
// 
//   // Second components are not equal.
//   x = std::make_tuple(A{0}, A{1}, A{2});
//   y = std::make_tuple(A{0}, A{-1}, A{2});
//   DRTEST_ASSERT(not is_equal(x, y));
// 
//   // Third components are not equal.
//   x = std::make_tuple(A{0}, A{1}, A{2});
//   y = std::make_tuple(A{0}, A{-1}, A{-2});
//   DRTEST_ASSERT(not is_equal(x, y));
// }
// 
// DRTEST_TEST(tupleSharedPolymorphic)
// {
//   auto is_equal = detail::IsEqual<
//       std::tuple<std::shared_ptr<A>, std::shared_ptr<A>, std::shared_ptr<A>>,
//       std::tuple<std::shared_ptr<B>, std::shared_ptr<B>, std::shared_ptr<B>>
//     >{};
//   std::tuple<std::shared_ptr<A>, std::shared_ptr<A>, std::shared_ptr<A>> x;
//   std::tuple<std::shared_ptr<A>, std::shared_ptr<A>, std::shared_ptr<A>> y;
// 
//   // First, second, third components are all equal.
//   x = std::make_tuple(
//       std::make_shared<B>(0, 1),
//       std::make_shared<B>(2, 3),
//       std::make_shared<B>(4, 5)
//     );
//   y = std::make_tuple(
//       std::make_shared<B>(0, 1),
//       std::make_shared<B>(2, 3),
//       std::make_shared<B>(4, 5)
//     );
//   DRTEST_ASSERT(is_equal(x, y));
// 
//   // First components are not equal.
// 
//   x = std::make_tuple(
//       std::make_shared<B>(0, 1),
//       std::make_shared<B>(2, 3),
//       std::make_shared<B>(4, 5)
//     );
//   y = std::make_tuple(
//       std::make_shared<B>(0, -1),
//       std::make_shared<B>(2, 3),
//       std::make_shared<B>(4, 5)
//     );
//   DRTEST_ASSERT(not is_equal(x, y));
// 
// 
//   // First component fails dynamic cast.
//   x = std::make_tuple(
//       std::make_shared<B>(0, 1),
//       std::make_shared<B>(2, 3),
//       std::make_shared<B>(4, 5)
//     );
//   y = std::make_tuple(
//       std::make_shared<A>(0),
//       std::make_shared<B>(2, 3),
//       std::make_shared<B>(4, 5)
//     );
//   DRTEST_ASSERT(not is_equal(x, y));
// 
// 
//   // Second components are not equal.
//   x = std::make_tuple(
//       std::make_shared<B>(0, 1),
//       std::make_shared<B>(2, 3),
//       std::make_shared<B>(4, 5)
//     );
//   y = std::make_tuple(
//       std::make_shared<B>(0, 1),
//       std::make_shared<B>(2, 234),
//       std::make_shared<B>(4, 5)
//     );
//   DRTEST_ASSERT(not is_equal(x, y));
// 
//   // Second component fails dynamic cast.
//   x = std::make_tuple(
//       std::make_shared<B>(0, 1),
//       std::make_shared<B>(2, 3),
//       std::make_shared<B>(4, 5)
//     );
//   y = std::make_tuple(
//       std::make_shared<B>(0, 1),
//       std::make_shared<A>(2),
//       std::make_shared<B>(4, 5)
//     );
//   DRTEST_ASSERT(not is_equal(x, y));
// 
//   // Third components are not equal.
//   x = std::make_tuple(
//       std::make_shared<B>(0, 1),
//       std::make_shared<B>(2, 3),
//       std::make_shared<B>(4, 5)
//     );
//   y = std::make_tuple(
//       std::make_shared<B>(0, 1),
//       std::make_shared<B>(2, 3),
//       std::make_shared<B>(4, 567)
//     );
//   DRTEST_ASSERT(not is_equal(x, y));
// 
//   // Third component fails dynamic cast.
//   x = std::make_tuple(
//       std::make_shared<B>(0, 1),
//       std::make_shared<B>(2, 3),
//       std::make_shared<B>(4, 5)
//     );
//   y = std::make_tuple(
//       std::make_shared<B>(0, 1),
//       std::make_shared<B>(2, 3),
//       std::make_shared<A>(4)
//     );
//   DRTEST_ASSERT(not is_equal(x, y));
// }
// 
// DRTEST_TEST(tupleRecursion)
// {
//   auto is_equal = detail::IsEqual<
//       std::tuple<int, std::tuple<int, std::tuple<float>>, double>
//     >{};
// 
//   // Middle tuple is equal.
//   auto x = std::make_tuple(1, std::make_tuple(2, std::make_tuple(3.4f)), 5.6);
//   auto y = std::make_tuple(1, std::make_tuple(2, std::make_tuple(3.4f)), 5.6);
//   DRTEST_ASSERT(is_equal(x, y));
// 
//   // Middle tuple is not equal.
//   x = std::make_tuple(1, std::make_tuple(2, std::make_tuple(3.4f)), 5.6);
//   y = std::make_tuple(1, std::make_tuple(3, std::make_tuple(3.4f)), 5.6);
//   DRTEST_ASSERT(not is_equal(x, y));
//   x = std::make_tuple(1, std::make_tuple(2, std::make_tuple(3.4f)), 5.6);
//   y = std::make_tuple(1, std::make_tuple(2, std::make_tuple(4.3f)), 5.6);
//   DRTEST_ASSERT(not is_equal(x, y));
// }
// 
// DRTEST_TEST(rawPointer)
// {
//   auto is_equal = detail::IsEqual<int*, int*>{};
//   int* x1 = new int{123};
//   int* x2 = new int{123};
//   DRTEST_ASSERT(is_equal(x1, x2));
//   delete x1;
//   delete x2;
// }
// 
// DRTEST_TEST(rawPointerConst)
// {
//   auto is_equal = detail::IsEqual<int* const, int* const>{};
//   int* const x1 = new int{123};
//   int* const x2 = new int{123};
//   DRTEST_ASSERT(is_equal(x1, x2));
//   delete x1;
//   delete x2;
// }
// 
// DRTEST_TEST(rawPointerToConst)
// {
//   auto is_equal = detail::IsEqual<int const*, int const*>{};
//   int const* x1 = new int{123};
//   int const* x2 = new int{123};
//   DRTEST_ASSERT(is_equal(x1, x2));
//   delete x1;
//   delete x2;
// }
// 
// DRTEST_TEST(rawPointerToPointer)
// {
//   auto is_equal = detail::IsEqual<int const**, int const**>{};
//   int const** x1 = new int const*{new int {123}};
//   int const** x2 = new int const*{new int {123}}; // p -> const p -> const i
//   DRTEST_ASSERT(is_equal(x1, x2));
//   delete *x1;
//   delete *x2;
//   delete x1;
//   delete x2;
// }
// 
// DRTEST_TEST(rawPointerToConstPointer)
// {
//   auto is_equal = detail::IsEqual<int const* const*, int const* const*>{};
//   int const* const* x1 = new int const*{new int {123}};
//   int const* const* x2 = new int const*{new int {123}}; // p -> const p -> const i
//   DRTEST_ASSERT(is_equal(x1, x2));
//   delete *x1;
//   delete *x2;
//   delete x1;
//   delete x2;
// }
// 
// DRTEST_TEST(derivedForBase)
// {
//   auto is_equal = detail::IsEqual<std::shared_ptr<A>>{};
//   auto p1 = std::make_shared<B>(1, 2);
//   auto p2 = std::make_shared<B>(3, 4);
//   DRTEST_ASSERT(not is_equal(p1, p2));
// }
