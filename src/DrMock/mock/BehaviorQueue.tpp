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

template<typename Class, typename ReturnType, typename... Args>
BehaviorQueue<Class, ReturnType, Args...>::BehaviorQueue()
:
  BehaviorQueue{std::make_shared<detail::WrapInSharedEqual<std::tuple<Args...>>>()}
{}

template<typename Class, typename ReturnType, typename... Args>
BehaviorQueue<Class, ReturnType, Args...>::BehaviorQueue(
    std::shared_ptr<detail::IWrapInSharedEqual<Args...>> wrap_in_shared_equal
  )
:
  wrap_in_shared_equal_{std::move(wrap_in_shared_equal)}
{}

template<typename Class, typename ReturnType, typename... Args>
Behavior<Class, ReturnType, Args...>&
BehaviorQueue<Class, ReturnType, Args...>::push()
{
  behaviors_.emplace_back(wrap_in_shared_equal_);
  return behaviors_.back();
}

template<typename Class, typename ReturnType, typename... Args>
Behavior<Class, ReturnType, Args...>&
BehaviorQueue<Class, ReturnType, Args...>::back()
{
  return behaviors_.back();
}

template<typename Class, typename ReturnType, typename... Args>
void
BehaviorQueue<Class, ReturnType, Args...>::enforce_order(bool value)
{
  enforce_order_ = value;
}

template<typename Class, typename ReturnType, typename... Args>
template<typename... Deriveds>
void
BehaviorQueue<Class, ReturnType, Args...>::polymorphic()
{
  wrap_in_shared_equal_ = std::make_shared<detail::WrapInSharedEqual<
      std::tuple<Args...>,
      std::tuple<Deriveds...>
    >>();
  for (auto& b : behaviors_)
  {
    b.template polymorphic<Deriveds...>();
  }
}

template<typename Class, typename ReturnType, typename... Args>
std::variant<
    std::monostate,
    std::pair<
        std::shared_ptr<typename std::decay<ReturnType>::type>,
        std::shared_ptr<AbstractSignal<Class>>
      >,
    std::exception_ptr
  >
BehaviorQueue<Class, ReturnType, Args...>::call(const Args&... args)
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
      return std::get<std::pair<
              std::shared_ptr<std::decay_t<ReturnType>>,
              std::shared_ptr<AbstractSignal<Class>>
        >>(result);
    }
  }
  else
  {
    return std::monostate{};
  }
}

template<typename Class, typename ReturnType, typename... Args>
bool
BehaviorQueue<Class, ReturnType, Args...>::is_exhausted() const
{
  return std::all_of(
      behaviors_.begin(), behaviors_.end(),
      [](const auto& x) { return x.is_exhausted(); }
    );
}

} // namespace drmock
