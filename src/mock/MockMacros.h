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

#ifndef DRMOCK_SRC_MOCKMACROS_H
#define DRMOCK_SRC_MOCKMACROS_H

/* Mock-specific macros
 * 
 * Beware! Some of the macros below allow an alternative interpretation,
 * which is used by defining DRMOCK_SWAP_MACROS. 
 *
 * DRMOCK_SWAP_MACROS should _never_ be defined in production code or
 * test code. It is for use in mock code only (that is, for use by the
 * DrMockGenerator). The reasoning behind this is that by using the
 * macros below, source code can be treated differently depending on
 * whether a mock object or an object proper is created. Consider
 * DRMOCK_SWAP_MACROS a _private_ macro of the DrMock module. */

// Ignore the argument during mock object compilation.
#ifndef DRMOCK_SWAP_MACROS
// #define DRMOCK_IGNORE(...) __VA_ARGS__  // Ignore the _macro_ in production/test code.
#define DRMOCK_IGNORE(...) __VA_ARGS__
#else
#define DRMOCK_IGNORE(...)  // Ignore the _arguments_ in mock code.
#endif

// Create a trivial operator== for argument in mock code.
//
// Note: The entire namespace of the class must be specified.
#ifndef DRMOCK_SWAP_MACROS
#define DRMOCK_DUMMY(...)  // Ignore the _arguments_ in production/test code.
#else
#define DRMOCK_DUMMY(cls) \
bool operator==(const cls&, const cls&) \
{ \
  return true; \
} // Define trivial operator==.
#endif

#endif /* DRMOCK_SRC_MOCKMACROS_H */
