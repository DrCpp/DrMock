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

#ifndef DRMOCK_SRC_DRMOCK_MOCK_SIGNAL_H
#define DRMOCK_SRC_DRMOCK_MOCK_SIGNAL_H

#include <DrMock/mock/AbstractSignal.h>

namespace drmock {

/**
 * Standard implementation of `AbstractSignal`.
 *
 * @tparam Parent The type of the object that emits the signal
 * @tparam Args... The function parameters of the signal
 */
template<typename Parent, typename... Args>
class Signal final : public AbstractSignal<Parent>
{
public:
  /**
   * @param signal A pointer to the signal
   * @param args The argument with which the signal is emitted
   */
  Signal(void (Parent::*signal)(Args...), Args... args);

  /**
   * See `AbstractSignal::invoke`.
   */
  void invoke(Parent*) override;

private:
  template<size_t... Is>
  void invoke_impl_(Parent*, const std::index_sequence<Is...>&);

  void (Parent::*signal_)(Args...);
  std::tuple<Args...> args_;
};

} // namespace drmock

#include "Signal.tpp"

#endif /* DRMOCK_SRC_DRMOCK_MOCK_SIGNAL_H */
