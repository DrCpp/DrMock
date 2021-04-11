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

#include "Singleton.h"

namespace drutility {

template<typename T>
std::shared_ptr<T>
Singleton<T>::get()
{
  std::lock_guard lck{Singleton<T>::mtx_()};
  return Singleton<T>::p_();
}

template<typename T>
void
Singleton<T>::set(std::shared_ptr<T> p)
{
  std::lock_guard lck{Singleton<T>::mtx_()};
  Singleton<T>::p_() = std::move(p);
}

template<typename T>
std::mutex&
Singleton<T>::mtx_()
{
  static std::mutex mtx{};
  return mtx;
}

template<typename T>
std::shared_ptr<T>&
Singleton<T>::p_()
{
  static std::shared_ptr<T> p{};
  return p;
}

} // namespace drutility
