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

#define DRTEST_USE_QT
#include <DrMock/Test.h>
#include "mock/UseQtMock.h"

#include <QEventLoop>

using namespace outer::inner;

DRTEST_TEST(eventLoop)
{
  auto foo = std::make_shared<UseQtMock>();
  auto bar = std::make_shared<UseQtMock>();

  QObject::connect(
      foo.get(), &IUseQt::theSignal,
      bar.get(), &IUseQt::theSlot,
      Qt::QueuedConnection  // Connect via event loop.
    );

  bar->mock.theSlot().push().times(1);
  emit foo->theSignal();

  QEventLoop event_loop{};
  event_loop.processEvents();

  DRTEST_ASSERT(bar->mock.control.verify());
}
