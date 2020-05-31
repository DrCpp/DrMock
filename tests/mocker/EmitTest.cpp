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

#define DRTEST_USE_QT  // For QSignalSpy!
#include "test/Test.h"
#include "mock/EmitMock.h"

#include <QSignalSpy>

using namespace outer::inner;

DRTEST_TEST(fails)
{
  EmitMock mock{};
  QSignalSpy spy{&mock, &IEmit::g};
  mock.f();
  spy.wait(100);
  DRTEST_ASSERT_EQ(spy.size(), 0);
}

DRTEST_TEST(succeeds)
{
  EmitMock mock{};
  mock.mock.f().push().emits(&IEmit::g);
  QSignalSpy spy{&mock, &IEmit::g};
  mock.f();
  if (spy.size() == 0)
  {
    spy.wait(100);
  }
  DRTEST_ASSERT_EQ(spy.size(), 1);
}

DRTEST_TEST(succeedsWithArgs)
{
  EmitMock mock{};
  QObject::connect(
      &mock, &IEmit::sig,
      &mock, &IEmit::slt
    );
  mock.mock.f().push().emits(&IEmit::sig, 123);
  mock.mock.slt().push().expects(123);
  mock.f();
  DRTEST_ASSERT(mock.mock.slt().verify());
}
