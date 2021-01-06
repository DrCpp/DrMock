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

#ifdef DRMOCK_USE_QT
#include <QObject>
#endif

namespace drmock {

template<typename Parent, typename... Args>
Signal<Parent, Args...>::Signal(void (Parent::*signal)(Args...), Args&&... args)
:
  signal_{std::move(signal)},
  args_{std::forward<Args>(args)...}
{}

template<typename Parent, typename... Args>
void
Signal<Parent, Args...>::invoke(Parent* parent)
{
  invoke_impl_(parent, std::make_index_sequence<sizeof...(Args)>{});
}

template<typename Parent, typename... Args>
template<size_t... Is>
void
Signal<Parent, Args...>::invoke_impl_(
    Parent* parent,
    const std::index_sequence<Is...>&
  )
{
#ifdef DRMOCK_USE_QT
  emit (parent->*signal_)(std::get<Is>(args_)...);
#else
  throw std::runtime_error{
      "Attempting to emit signal without defining DRMOCK_USE_QT"
    };
#endif
}

} // namespace drmock
