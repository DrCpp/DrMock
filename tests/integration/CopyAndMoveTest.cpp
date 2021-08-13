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
#include "mock/CopyAndMoveMock.h"

using namespace outer::inner;

DRTEST_TEST(success)
{
  {
    CopyAndMoveMock mock{};
    mock.mock.byValue().push()
        .expects(NotCopyConstructible{1})
        .times(1);
    mock.byValue(NotCopyConstructible{1});
    DRTEST_ASSERT(mock.mock.byValue().verify());
  }

  {
    CopyAndMoveMock mock{};
    mock.mock.lValueRef().push()
        .expects(NotCopyConstructible{2})
        .times(1);
    mock.lValueRef(NotCopyConstructible{2});
    DRTEST_ASSERT(mock.mock.lValueRef().verify());
  }

  {
    CopyAndMoveMock mock{};
    mock.mock.rValueRef().push()
        .expects(NotCopyConstructible{2})
        .times(1);
    mock.rValueRef(NotCopyConstructible{2});
    DRTEST_ASSERT(mock.mock.rValueRef().verify());
  }
}
