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

namespace drmock { namespace detail {

template<typename T, typename ReturnType>
Effect<T, ReturnType>::Effect(std::shared_ptr<ReturnType> return_value)
:
  Effect(std::move(return_value), {})
{}

template<typename T, typename ReturnType>
Effect<T, ReturnType>::Effect(std::shared_ptr<AbstractSignal<T>> signal)
:
  Effect({}, std::move(signal))
{}

template<typename T, typename ReturnType>
Effect<T, ReturnType>::Effect(
    std::shared_ptr<ReturnType> return_value,
    std::shared_ptr<AbstractSignal<T>> signal
  )
:
  return_value_{std::move(return_value)},
  signal_{std::move(signal)}
{}

template<typename T, typename ReturnType>
Effect<T, ReturnType>::Effect(std::exception_ptr exception)
:
  exception_{std::move(exception)}
{}

template<typename T, typename ReturnType>
bool
Effect<T, ReturnType>::failed(std::exception_ptr exception) const
{
  return !return_value_ and !signal_ and !exception_;
}

template<typename T, typename ReturnType>
std::shared_ptr<ReturnType>
Effect<T, ReturnType>::deploy(T* parent)
{
  if (exception_)
  {
    std::rethrow_exception(exception_);
  }
  if (signal_)
  {
    signal_->invoke(parent);
  }
  return return_value_;
}

}} // namespace drmock::detail
