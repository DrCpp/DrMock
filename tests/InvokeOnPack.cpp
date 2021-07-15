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

#include <DrMock/Test.h>
#include <DrMock/mock/detail/InvokeOnPack.h>
#include <DrMock/mock/Equal.h>
#include <DrMock/mock/IMatcher.h>

using namespace drmock;

DRTEST_TEST(testStandard)
{
  detail::InvokeOnPack<std::tuple<int, std::string>> invoke_on_pack{};
  auto equal1 = std::make_shared<Equal<int>>(12);
  auto equal2 = std::make_shared<Equal<std::string>>("12");
  auto t = std::make_tuple(equal1, equal2);
  DRTEST_ASSERT(invoke_on_pack(t, 12, std::string{"12"}));
  DRTEST_ASSERT(not invoke_on_pack(t, 13, std::string{"12"}));
  DRTEST_ASSERT(not invoke_on_pack(t, 12, std::string{"13"}));
}

DRTEST_TEST(testNonPolymorphicPointer)
{
  detail::InvokeOnPack<std::tuple<
      std::shared_ptr<int>,
      std::unique_ptr<int>,
      int*
    >> invoke_on_pack{};
  auto t = std::make_tuple(
          std::make_shared<Equal<std::shared_ptr<int>>>(std::make_shared<int>(12)),
          std::make_shared<Equal<std::unique_ptr<int>>>(std::make_unique<int>(13)),
          std::make_shared<Equal<int*>>(new int{14})
        );
  DRTEST_ASSERT(invoke_on_pack(
      t,
      std::make_shared<int>(12),
      std::make_unique<int>(13),
      new int{14}
    ));
  DRTEST_ASSERT(not invoke_on_pack(
      t,
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
    detail::InvokeOnPack<
        std::tuple<
            std::shared_ptr<A>,
            std::unique_ptr<A>
          >
      > invoke_on_pack{};
    DRTEST_ASSERT(invoke_on_pack(
        std::make_tuple(
            std::make_shared<Equal<std::shared_ptr<A>>>(std::make_shared<A>(12)),
            std::make_shared<Equal<std::unique_ptr<A>, std::unique_ptr<B>>>(std::unique_ptr<B>(new B{1, 2}))
          ),
        std::make_shared<A>(12),
        std::unique_ptr<A>(new B{2, 2})
      ));
  }

  {
    detail::InvokeOnPack<
        std::tuple<
            std::shared_ptr<A>,
            std::unique_ptr<A>
          >
      > invoke_on_pack{};
    DRTEST_ASSERT(invoke_on_pack(
        std::make_tuple(
            std::make_shared<Equal<std::shared_ptr<A>, std::shared_ptr<B>>>(std::shared_ptr<B>(new B{1, 2})),
            std::make_shared<Equal<std::unique_ptr<A>>>(std::make_unique<A>(12))
          ),
        std::shared_ptr<A>(new B{2, 2}),
        std::make_unique<A>(12)
      ));
  }
}

DRTEST_TEST(testEmptyTuple)
{
  detail::InvokeOnPack<std::tuple<>> invoke_on_pack{};
  DRTEST_ASSERT(invoke_on_pack(std::tuple<>{}));
}

DRTEST_TEST(derivedForBase)
{
  detail::InvokeOnPack<std::tuple<std::shared_ptr<A>>> invoke_on_pack{};
  auto p1 = std::make_shared<B>(1, 2);
  auto p2 = std::make_shared<B>(3, 4);
  auto p1_2 = std::make_shared<B>(1, 2);
  DRTEST_ASSERT(not invoke_on_pack(
      std::make_tuple(std::make_shared<Equal<std::shared_ptr<A>>>(p1)),
      p2
    ));
  DRTEST_ASSERT(invoke_on_pack(
      std::make_tuple(std::make_shared<Equal<std::shared_ptr<A>>>(p1)),
      p1_2
    ));
}
