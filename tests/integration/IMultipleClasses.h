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

#ifndef DRMOCK_TESTS_INTEGRATION_IMULTIPLECLASSES_H
#define DRMOCK_TESTS_INTEGRATION_IMULTIPLECLASSES_H

namespace outer { namespace inner {

class IFoo
{
public:
  virtual ~IFoo() = default;

  virtual void f() = 0;
};

// Doesn't match the regex, should be ignored.
class Bar
{
public:
  // Lots of offensive code:
  void f(int x) const { int y = x + 3; (void)y; return; }
};

}} // namespace outer::inner

#endif /* DRMOCK_TESTS_INTEGRATION_IMULTIPLECLASSES_H */
