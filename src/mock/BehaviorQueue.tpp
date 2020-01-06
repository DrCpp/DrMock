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

namespace drmock {

template<typename Result, typename... Args>
BehaviorQueue<Result, Args...>::BehaviorQueue()
:
  BehaviorQueue{std::make_shared<detail::IsTuplePackEqual<std::tuple<Args...>>>()}
{}

template<typename Result, typename... Args>
BehaviorQueue<Result, Args...>::BehaviorQueue(
    std::shared_ptr<detail::IIsTuplePackEqual<Args...>> is_tuple_pack_equal
  )
:
  is_tuple_pack_equal_{std::move(is_tuple_pack_equal)}
{}

template<typename Result, typename... Args>
Behavior<Result, Args...>&
BehaviorQueue<Result, Args...>::push()
{
  behaviors_.emplace_back(is_tuple_pack_equal_);
  return behaviors_.back();
}

template<typename Result, typename... Args>
Behavior<Result, Args...>&
BehaviorQueue<Result, Args...>::back()
{
  return behaviors_.back();
}

template<typename Result, typename... Args>
void
BehaviorQueue<Result, Args...>::enforce_order(bool value)
{
  enforce_order_ = value;
}

template<typename Result, typename... Args>
template<typename... Deriveds>
void
BehaviorQueue<Result, Args...>::polymorphic()
{
  setIsEqual(std::make_shared<detail::IsTuplePackEqual<
      std::tuple<Args...>,
      std::tuple<Deriveds...>
    >>());
}

template<typename Result, typename... Args>
void
BehaviorQueue<Result, Args...>::setIsEqual(
    std::shared_ptr<detail::IIsTuplePackEqual<Args...>> is_tuple_pack_equal
  )
{
  for (auto& b : behaviors_)
  {
    b.setIsEqual(is_tuple_pack_equal);
  }
  is_tuple_pack_equal_ = std::move(is_tuple_pack_equal);
}

template<typename Result, typename... Args>
std::variant<
    std::monostate,
    std::shared_ptr<typename BehaviorQueue<Result, Args...>::DecayedResult>,
    std::exception_ptr
  >
BehaviorQueue<Result, Args...>::call(const Args&... args)
{
  auto match = behaviors_.end();
  auto begin = std::find_if(
      behaviors_.begin(), behaviors_.end(),
      [] (const auto& behavior) { return behavior.is_persistent(); }
    );
  if (begin != behaviors_.end())
  {
    if (enforce_order_)
    {
      if (begin->match(args...))
      {
        match = begin;
      }
    }
    else
    {
      match = std::find_if(
          begin, behaviors_.end(),
          [&args...] (const auto& behavior) { return behavior.match(args...); }
        );
    }
  }

  if (match != behaviors_.end())
  {
    auto result = match->produce();

    if (std::holds_alternative<std::exception_ptr>(result))
    {
      return std::get<std::exception_ptr>(result);
    }
    else
    {
      return std::get<std::shared_ptr<DecayedResult>>(result);
    }
  }
  else
  {
    return std::monostate{};
  }
}

template<typename Result, typename... Args>
bool
BehaviorQueue<Result, Args...>::is_exhausted() const
{
  bool value = true;
  for (const auto& b : behaviors_)
  {
    value = value and b.is_exhausted();
  }
  return value;
}

} // namespace drmock
