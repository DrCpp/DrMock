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

#ifndef DRMOCK_SRC_MOCK_SIGNAL_H
#define DRMOCK_SRC_MOCK_SIGNAL_H

#include "AbstractSignal.h"

namespace drmock {

/* Signal

Class template that represents a Qt signal emit (in fact, a method call).

The signal is ctor'ed from a member function pointer to the signal and
the arguments of the emit. Note the lack of any reference to the
invoking QObject.

The invoking QObject occurs only in the `invoke` method call, which will
invoke/execute the `emit`.
*/

template<typename Parent, typename... Args>
class Signal final : public AbstractSignal<Parent>
{
public:
  Signal(void (Parent::*)(Args...), Args&&...);
  void invoke(Parent*) override;

private:
  template<size_t... Is>
  void invoke_impl_(Parent*, const std::index_sequence<Is...>&);

  void (Parent::*signal_)(Args...);
  std::tuple<Args...> args_;
};

} // namespace drmock

#include "Signal.tpp"

#endif /* DRMOCK_SRC_MOCK_SIGNAL_H */
