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

#ifndef DRMOCK_SRC_MOCK_MOCKMACROS_H
#define DRMOCK_SRC_MOCK_MOCKMACROS_H

/* Mock-specific macros
 *
 * Beware! This header should _never_ be used in production code!
 * It is for use in mock code only (that is, for use by the
 * DrMockGenerator), and will be included in the source code of mock
 * objects.
 *
 * To apply macros _only_ in mock code, protect them using
 *
 * #ifdef DRMOCK
 * ...
 * #endif
 *
 * This way, header files can be treated differently depending
 * on whether a mock object or an object proper is created. */

// The macro DRMOCK is defined if and only if this header is included.
#define DRMOCK

// Create a trivial operator== for argument.
//
// Note: The entire namespace of the class must be specified.
#define DRMOCK_DUMMY(cls) \
bool operator==(const cls&, const cls&) \
{ \
  return true; \
} // Define trivial operator==.

#endif /* DRMOCK_SRC_MOCK_MOCKMACROS_H */
