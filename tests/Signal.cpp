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

// #define DRTEST_USE_QT
#define DRMOCK_USE_QT  // This is usually set by the mocker.
#include "test/Test.h"
#include "mock/Signal.h"
#include "Dummy.h"

// FIXME Check that the correct arguments are forwarded to the signal in
// the tests below.

using namespace drmock;

template<typename ReturnType>
using Result = std::pair<
    std::shared_ptr<ReturnType>,
    std::shared_ptr<AbstractSignal<Dummy>>
  >;

DRTEST_TEST(invokeNoParameters)
{
  Dummy dummy{Qt::DirectConnection};
  Signal<Dummy> signal{&Dummy::no_params};

  QSignalSpy spy{&dummy, &Dummy::no_params};
  DRTEST_ASSERT(spy.isValid());
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
  Dummy dummy{Qt::DirectConnection};
  Signal<Dummy, int, const QString&> signal{
      &Dummy::multiple_params,
      3, QString{"foo"}
    };
  QSignalSpy spy{&dummy, &Dummy::multiple_params};
  DRTEST_ASSERT(spy.isValid());
  DRTEST_ASSERT_EQ(spy.size(), 0);

  signal.invoke(&dummy);
  if (spy.size() == 0)
  {
    spy.wait(100);
  }
  DRTEST_ASSERT_EQ(spy.size(), 1);
}

DRTEST_TEST(invokeWithReference)
{
  Dummy dummy{Qt::DirectConnection};
  std::string foo = "bar";
  Signal<Dummy, std::string&> signal{&Dummy::pass_by_ref, foo};
  QSignalSpy spy{&dummy, &Dummy::pass_by_ref};
  DRTEST_ASSERT(spy.isValid());
  DRTEST_ASSERT_EQ(spy.size(), 0);

  signal.invoke(&dummy);
  if (spy.size() == 0)
  {
    spy.wait(100);
  }
  DRTEST_ASSERT_EQ(spy.size(), 1);
}
