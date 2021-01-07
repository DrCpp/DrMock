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

#ifndef DRMOCK_SRC_MOCK_ABSTRACTSIGNAL_H
#define DRMOCK_SRC_MOCK_ABSTRACTSIGNAL_H

namespace drmock {

/* AbstractSignal

Class template that represents a Qt signal emit (in fact, a method
call). The `invoke(Parent* parent)` method invokes/executes the emit on
the QObject `parent`.
*/

template<typename Parent>
class AbstractSignal
{
public:
  virtual ~AbstractSignal() = default;

  virtual void invoke(Parent*) = 0;
};

} // namespace drmock

#endif /* DRMOCK_SRC_MOCK_ABSTRACTSIGNAL_H */
