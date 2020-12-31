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

#include <string>
#include <memory>

#include "test/Test.h"
#include "utility/Singleton.h"
#include "utility/Singleton.tpp"

using namespace drutility;

struct HasNoDefault
{
};

struct HasDefault
{
  virtual ~HasDefault() = default;

  virtual std::string func() const = 0;
};

struct ThisIsTheDefault : public HasDefault
{
  std::string func() const override
  {
    return "implemented";
  }
};

template<>
struct drutility::SingletonDefault<HasDefault>
{
  using type = ThisIsTheDefault;
};

template class drutility::Singleton<HasNoDefault>;
template class drutility::Singleton<HasDefault>;

DRTEST_TEST(no_default_if_none_set)
{
  DRTEST_ASSERT(not Singleton<HasNoDefault>::get());
}

DRTEST_TEST(default_if_set)
{
  DRTEST_ASSERT(Singleton<HasDefault>::get());
  DRTEST_ASSERT_EQ(Singleton<HasDefault>::get()->func(), std::string{"implemented"});
}
