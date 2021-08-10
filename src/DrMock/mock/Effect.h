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

#ifndef DRMOCK_SRC_DRMOCK_MOCK_DETAIL_EFFECT_H
#define DRMOCK_SRC_DRMOCK_MOCK_DETAIL_EFFECT_H

#include <memory>

#include <DrMock/mock/AbstractSignal.h>

namespace drmock {

template<typename T, typename ReturnType>
class Effect
{
public:
  Effect() = default();
  Effect(std::shared_ptr<ReturnType>);
  Effect(std::shared_ptr<AbstractSignal<T>>);
  Effect(std::shared_ptr<ReturnType>, std::shared_ptr<AbstractSignal<T>>);
  Effect(std::exception_ptr);

  bool failed() const;
  std::shared_ptr<ReturnType> return_value();
  std::shared_ptr<AbstractSignal<T>> signal();
  std::exception_ptr exception();

private:
  std::shared_ptr<ReturnType>> return_value_{};
  std::shared_ptr<AbstractSignal<T>> signal_{};
  std::exception_ptr exception_{};
};

} // namespace drmock

#include "Effect.tpp"

#endif /* DRMOCK_SRC_DRMOCK_MOCK_DETAIL_EFFECT_H */
