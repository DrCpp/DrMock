/* Copyright 2020 Ole Kliemann, Malte Kliemann
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

#include <QSignalSpy>

#define DRTEST_USE_QT
#define DRMOCK_USE_QT
#include "test/Test.h"
#include "mock/Signal.h"
#include "Dummy.h"

using namespace drmock;

template<typename ReturnType>
using Result = std::pair<
    std::shared_ptr<ReturnType>,
    std::shared_ptr<AbstractSignal<Dummy>>
  >;

DRTEST_TEST(invokeNoParameters)
{
  Dummy dummy{};
  Signal<Dummy> signal{&Dummy::no_params};
  QSignalSpy spy{&dummy, &Dummy::no_params};
  DRTEST_ASSERT_EQ(spy.size(), 0);

  signal.invoke(&dummy);
  if (spy.size() == 0)
  {
    spy.wait(100);
  }
  DRTEST_ASSERT_EQ(spy.size(), 1);
}

DRTEST_TEST(invokeWithParameters)
{
  Dummy dummy{};
  float f = 1.23f;
  std::vector<int> v = {1, 2, 3};
  Signal<Dummy, int, float&, const std::vector<int>&> signal{
      &Dummy::multiple_params,
      123, f, v
    };
  QSignalSpy spy{&dummy, &Dummy::multiple_params};
  DRTEST_ASSERT_EQ(spy.size(), 0);

  signal.invoke(&dummy);
  if (spy.size() == 1)
  {
    spy.wait(100);
  }
  DRTEST_ASSERT_EQ(spy.size(), 1);
  // FIXME Check arguments passed?
}

DRTEST_TEST(invokeWithReference)
{
  Dummy dummy{};
  std::string foo = "bar";
  Signal<Dummy, std::string&> signal{&Dummy::pass_by_ref, foo};
  QSignalSpy spy{&dummy, &Dummy::pass_by_ref};
  DRTEST_ASSERT_EQ(spy.size(), 0);

  QObject::connect(
      &dummy, &Dummy::pass_by_ref,
      &dummy, &Dummy::bazzify
    );

  signal.invoke(&dummy);
  if (spy.size() == 1)
  {
    spy.wait(100);
  }
  DRTEST_ASSERT_EQ(spy.size(), 1);
  DRTEST_ASSERT_EQ(foo, "baz");
}
