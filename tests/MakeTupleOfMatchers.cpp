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
#include <DrMock/mock/Equal.h>
#include <DrMock/mock/detail/MatchPack.h>
#include <DrMock/mock/detail/MakeTupleOfMatchers.h>

using namespace drmock;

DRTEST_TEST(wrap_builtin)
{
  detail::MakeTupleOfMatchers<std::tuple<int, float, std::string>> wrapper{};
  auto match_pack = detail::MatchPack<std::tuple<int, float, std::string>>{};
  auto eq1 = wrapper.wrap(123, 1.23f, "foo");
  auto eq2 = wrapper.wrap(std::get<0>(eq1), std::get<1>(eq1), std::get<2>(eq1));  // Rewrap.
  auto eq3 = wrapper.wrap(123, std::make_shared<Equal<float>>(1.23), "foo");  // Mixed.
  DRTEST_ASSERT(match_pack(eq1, 123, 1.23f, "foo"));
  DRTEST_ASSERT(match_pack(eq2, 123, 1.23f, "foo"));
  DRTEST_ASSERT(match_pack(eq3, 123, 1.23f, "foo"));
  DRTEST_ASSERT(not match_pack(eq1, 234, 1.23f, "foo"));
  DRTEST_ASSERT(not match_pack(eq1, 123, 2.34f, "foo"));
  DRTEST_ASSERT(not match_pack(eq1, 123, 1.23f, "bar"));
  DRTEST_ASSERT(not match_pack(eq2, 234, 1.23f, "foo"));
  DRTEST_ASSERT(not match_pack(eq2, 123, 2.34f, "foo"));
  DRTEST_ASSERT(not match_pack(eq2, 123, 1.23f, "bar"));
  DRTEST_ASSERT(not match_pack(eq3, 234, 1.23f, "foo"));
  DRTEST_ASSERT(not match_pack(eq3, 123, 2.34f, "foo"));
  DRTEST_ASSERT(not match_pack(eq3, 123, 1.23f, "bar"));
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

DRTEST_DATA(wrap_shared_polymorphic)
{
  drtest::addColumn<std::shared_ptr<A>>("actual1");
  drtest::addColumn<std::shared_ptr<A>>("actual2");
  drtest::addColumn<std::shared_ptr<A>>("actual3");
  drtest::addColumn<std::shared_ptr<A>>("expected1");
  drtest::addColumn<std::shared_ptr<A>>("expected2");
  drtest::addColumn<std::shared_ptr<A>>("expected3");
  drtest::addColumn<bool>("result");
  drtest::addRow(
      "all eq",
      std::dynamic_pointer_cast<A, B>(std::make_shared<B>(0, 1)),
      std::dynamic_pointer_cast<A, B>(std::make_shared<B>(2, 3)),
      std::dynamic_pointer_cast<A, B>(std::make_shared<B>(3, 4)),
      std::dynamic_pointer_cast<A, B>(std::make_shared<B>(0, 1)),
      std::dynamic_pointer_cast<A, B>(std::make_shared<B>(2, 3)),
      std::dynamic_pointer_cast<A, B>(std::make_shared<B>(3, 4)),
      true
    );
}

DRTEST_TEST(wrap_shared_polymorphic)
{
  DRTEST_FETCH(std::shared_ptr<A>, actual1);
  DRTEST_FETCH(std::shared_ptr<A>, actual2);
  DRTEST_FETCH(std::shared_ptr<A>, actual3);
  DRTEST_FETCH(std::shared_ptr<A>, expected1);
  DRTEST_FETCH(std::shared_ptr<A>, expected2);
  DRTEST_FETCH(std::shared_ptr<A>, expected3);
  DRTEST_FETCH(bool, result);

  detail::MakeTupleOfMatchers<
      std::tuple<std::shared_ptr<A>, std::shared_ptr<A>, std::shared_ptr<A>>,
      std::tuple<std::shared_ptr<B>, std::shared_ptr<B>, std::shared_ptr<B>>
    > wrapper{};
  auto eq = wrapper.wrap(actual1, actual2, actual3);
  detail::MatchPack<
      std::tuple<std::shared_ptr<A>, std::shared_ptr<A>, std::shared_ptr<A>>
    > match_pack{};
  DRTEST_ASSERT_EQ(match_pack(eq, expected1, expected2, expected3), result);
}
