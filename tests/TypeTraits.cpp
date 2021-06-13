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

#include "test/Test.h"
#include "mock/detail/TypeTraits.h"

class Base {};
class Derived : public Base {};
class Other {};

using namespace drmock::detail;

DRTEST_TEST(is_base_of_smart_pointer_shared)
{
  using base = std::shared_ptr<Base>;
  using derived = std::shared_ptr<Derived>;
  using other = std::shared_ptr<Other>;
  DRTEST_ASSERT((is_base_of_smart_ptr_v<base, base>));
  DRTEST_ASSERT((is_base_of_smart_ptr_v<base, derived>));
  DRTEST_ASSERT((not is_base_of_smart_ptr_v<derived, base>));
  DRTEST_ASSERT((not is_base_of_smart_ptr_v<other, base>));
  DRTEST_ASSERT((not is_base_of_smart_ptr_v<base, other>));
}

DRTEST_TEST(is_base_of_smart_pointer_unique)
{
  using base = std::unique_ptr<Base>;
  using derived = std::unique_ptr<Derived>;
  using other = std::unique_ptr<Other>;
  DRTEST_ASSERT((is_base_of_smart_ptr_v<base, base>));
  DRTEST_ASSERT((is_base_of_smart_ptr_v<base, derived>));
  DRTEST_ASSERT((not is_base_of_smart_ptr_v<derived, base>));
  DRTEST_ASSERT((not is_base_of_smart_ptr_v<other, base>));
  DRTEST_ASSERT((not is_base_of_smart_ptr_v<base, other>));
}

DRTEST_TEST(is_base_of_tuple)
{
  using base = std::unique_ptr<Base>;
  using derived = std::unique_ptr<Derived>;
  using other = std::unique_ptr<Other>;

  DRTEST_ASSERT((is_base_of_tuple_v<std::tuple<base, base>, std::tuple<base, base>>));
  DRTEST_ASSERT((is_base_of_tuple_v<std::tuple<base, base>, std::tuple<derived, base>>));
  DRTEST_ASSERT((is_base_of_tuple_v<std::tuple<base, base>, std::tuple<base, derived>>));
  DRTEST_ASSERT((is_base_of_tuple_v<std::tuple<base, base>, std::tuple<derived, derived>>));

  DRTEST_ASSERT((not is_base_of_tuple_v<std::tuple<derived, base>, std::tuple<base, base>>));
  DRTEST_ASSERT((not is_base_of_tuple_v<std::tuple<base, derived>, std::tuple<base, base>>));
  DRTEST_ASSERT((not is_base_of_tuple_v<std::tuple<derived, derived>, std::tuple<base, base>>));

  DRTEST_ASSERT((not is_base_of_tuple_v<std::tuple<base, base>, std::tuple<other, base>>));
  DRTEST_ASSERT((not is_base_of_tuple_v<std::tuple<base, base>, std::tuple<base, other>>));
  DRTEST_ASSERT((not is_base_of_tuple_v<std::tuple<base, base>, std::tuple<other, other>>));
}
