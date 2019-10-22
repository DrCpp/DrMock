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

#include "mock/detail/Hash.h"

using namespace drmock;

class T
{
public:
  T(int x = 0) : x_{x} {}
  virtual ~T() = default;
  T& operator=(const T& rhs) 
  { 
    x_ = rhs.x_; 
    return *this;
  }
  bool operator==(const T& rhs) const { return x_ == rhs.x_; }
  friend std::ostream& operator<<(std::ostream& os, const T& rhs)
  {
    return (os << rhs.x_);
  }
  int x() const { return x_; }
private:
  int x_;
};

class U : public T
{
public:
  U(int y = 0) : T{0}, y_{y} {}
  int y() const { return y_; }
private:
  int y_;
};

template<>
struct std::hash<T>
{
  std::size_t operator()(const T& t) const
  {
    return t.x();
  }
};

template<>
struct std::hash<U>
{
  std::size_t operator()(const U& u) const
  {
    return u.y();
  }
};

template<typename T, typename U>
struct MHash : public detail::IHash<T, U>
{
  MHash(std::size_t h)
  :
    h_{h}
  {}
  std::size_t operator()(const T& x) const override
  {
    x_ = x;
    return h_;
  }
  T x() const { return x_; }

private:
  mutable T x_;
  std::size_t h_;
};

// Raw types.
DRTEST_TEST(obj)
{
  auto hash = detail::Hash<T>{};
  DRTEST_COMPARE(
      hash(T{123}),
      123
    );
}

// Smart pointers.
DRTEST_TEST(ptr)
{
  auto mhash = std::make_shared<MHash<T, T>>(123);
  auto hash = detail::Hash<std::shared_ptr<T>>(mhash);
  DRTEST_COMPARE(
      hash(std::make_shared<T>(321)),
      123
    );
  DRTEST_COMPARE(
      mhash->x(),
      T{321}
    );
}

DRTEST_TEST(polymorphic)
{
  auto mhash = std::make_shared<MHash<U, U>>(123);
  auto hash = detail::Hash<std::shared_ptr<T>, std::shared_ptr<U>>(mhash);
  // Dynamic cast success
  DRTEST_COMPARE(
      hash(std::make_shared<U>(456)),
      123
    );
  DRTEST_COMPARE(
      mhash->x(),
      U{456}
    );
  // Dynamic cast failure
  DRTEST_ASSERT_THROW(
      hash(std::make_shared<T>(456)),
      std::logic_error
    );
}

DRTEST_TEST(tuple)
{
  auto mhash = std::make_tuple(
      std::make_shared<MHash<std::shared_ptr<T>, std::shared_ptr<U>>>(12),
      std::make_shared<MHash<std::shared_ptr<int>, std::shared_ptr<int>>>(34),
      std::make_shared<MHash<T, T>>(56)
    );
  auto hash = detail::Hash<
      std::tuple<
          std::shared_ptr<T>, 
          std::shared_ptr<int>, 
          T
        >,
      std::tuple<
          std::shared_ptr<U>,
          std::shared_ptr<int>,
          T
        >
    >(mhash);
  DRTEST_COMPARE(
      hash(
          std::make_tuple(
              std::make_shared<T>(65), 
              std::make_shared<int>(43), 
              T{21}
            )
        ),
      (12 ^ 34 ^ 56)
    );
  DRTEST_COMPARE(
      *std::get<0>(mhash)->x(),
      T{65}
    );
  DRTEST_COMPARE(
      *std::get<1>(mhash)->x(),
      43
    );
  DRTEST_COMPARE(
      std::get<2>(mhash)->x(),
      T{21}
    );
}
