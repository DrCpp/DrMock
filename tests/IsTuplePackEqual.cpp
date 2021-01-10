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
#include <DrMock/mock/detail/IsTuplePackEqual.h>

using namespace drmock::detail;

DRTEST_TEST(testStandard)
{
  IsTuplePackEqual<std::tuple<int, std::string>> is_tuple_pack_equal{};
  DRTEST_ASSERT(is_tuple_pack_equal(
      std::make_tuple<int, std::string>(12, "12"),
      12, std::string{"12"}
    ));
  DRTEST_ASSERT(not is_tuple_pack_equal(
      std::make_tuple<int, std::string>(12, "12"),
      13, std::string{"12"}
    ));
  DRTEST_ASSERT(not is_tuple_pack_equal(
      std::make_tuple<int, std::string>(12, "12"),
      12, std::string{"13"}
    ));
}

DRTEST_TEST(testNonPolymorphicPointer)
{
  IsTuplePackEqual<std::tuple<
      std::shared_ptr<int>,
      std::unique_ptr<int>,
      int*
    >> is_tuple_pack_equal{};
  DRTEST_ASSERT(is_tuple_pack_equal(
      std::make_tuple(
          std::make_shared<int>(12),
          std::make_unique<int>(13),
          new int{14}
        ),
      std::make_shared<int>(12),
      std::make_unique<int>(13),
      new int{14}
    ));
  DRTEST_ASSERT(not is_tuple_pack_equal(
      std::make_tuple(
          std::make_shared<int>(12),
          std::make_unique<int>(13),
          new int{14}
        ),
      std::make_shared<int>(12),
      std::make_unique<int>(14),
      new int{13}
    ));
}

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

DRTEST_TEST(testPolymorphicPointer)
{
  {
    IsTuplePackEqual<
        std::tuple<
            std::shared_ptr<A>,
            std::unique_ptr<A>
          >,
        std::tuple<
            std::shared_ptr<A>,
            std::unique_ptr<B>
          >
      > is_tuple_pack_equal{};
    DRTEST_ASSERT(is_tuple_pack_equal(
        std::make_tuple(
            std::make_shared<A>(12),
            std::unique_ptr<A>(new B{1, 2})
          ),
        std::make_shared<A>(12),
        std::unique_ptr<A>(new B{2, 2})
      ));
  }

  {
    IsTuplePackEqual<
        std::tuple<
            std::shared_ptr<A>,
            std::unique_ptr<A>
          >,
        std::tuple<
            std::shared_ptr<B>,
            std::unique_ptr<A>
          >
      > is_tuple_pack_equal{};
    DRTEST_ASSERT(is_tuple_pack_equal(
        std::make_tuple(
            std::shared_ptr<A>(new B{1, 2}),
            std::make_unique<A>(12)
          ),
        std::shared_ptr<A>(new B{2, 2}),
        std::make_unique<A>(12)
      ));
  }
}

DRTEST_TEST(testEmptyTuple)
{
  IsTuplePackEqual<std::tuple<>> is_tuple_pack_equal{};
  DRTEST_ASSERT(is_tuple_pack_equal(std::tuple<>{}));
}

DRTEST_TEST(derivedForBase)
{
  IsTuplePackEqual<std::tuple<std::shared_ptr<A>>> is_tuple_pack_equal{};
  auto p1 = std::make_shared<B>(1, 2);
  auto p2 = std::make_shared<B>(3, 4);
  auto p1_2 = std::make_shared<B>(1, 2);
  DRTEST_ASSERT(not is_tuple_pack_equal(std::make_tuple<std::shared_ptr<A>>(p1), p2));
  DRTEST_ASSERT(is_tuple_pack_equal(std::make_tuple<std::shared_ptr<A>>(p1), p1_2));
}
