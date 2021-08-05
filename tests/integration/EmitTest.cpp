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
#include <DrMock/Test.h>
#include "mock/EmitMock.h"

#include <QSignalSpy>

// Test that the mocker correctly sets each method's parent but running
// a general functional test of the `emits` feature.

using namespace outer::inner;

DRTEST_TEST(succeedsWithArgs)
{
  EmitMock mock{};
  QObject::connect(
      &mock, &IEmit::signal,
      &mock, &IEmit::slot
    );
  std::string foo = "bar";
  std::vector<float> x = {1.2f, 3.4f, 5.6f};
  mock.mock.go().push().emits<int, std::string&, const std::vector<float>&>(
      &IEmit::signal,
      123, foo, x
    );
  foo = "baz";  // Change foo to test that references are not copied.
  mock.mock.slot().push().expects(123, foo, x);
  mock.go();
  DRTEST_ASSERT(mock.mock.slot().verify());
}

DRTEST_TEST(failsWithArgs)
{
  EmitMock mock{};
  QObject::connect(
      &mock, &IEmit::signal,
      &mock, &IEmit::slot
    );
  std::string foo = "bar";
  std::vector<float> x = {1.2f, 3.4f, 5.6f};
  mock.mock.go().push().emits<int, std::string&, const std::vector<float>&>(
      &IEmit::signal,
      123, foo, x
    );
  foo = "baz";  // Change foo to test that references are not copied.
  mock.mock.slot().push().expects(123, foo, x);  // Expect wrong arguments.
  mock.go();
  DRTEST_ASSERT(mock.mock.slot().verify());
}
