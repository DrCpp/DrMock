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

#include <DrMock/Test.h>
#include <DrMock/mock/StateObject.h>

using namespace drmock;

DRTEST_TEST(testStateObject)
{
  StateObject so{};
  DRTEST_COMPARE(so.get(), std::string{});
  DRTEST_COMPARE(so.get("some slot 1"), std::string{});
  DRTEST_COMPARE(so.get("some slot 2"), std::string{});
  so.set("test");
  DRTEST_COMPARE(so.get(), std::string{"test"});
  DRTEST_COMPARE(so.get("some slot 1"), std::string{});
  DRTEST_COMPARE(so.get("some slot 2"), std::string{});
  so.set("some slot 1", "test 1");
  DRTEST_COMPARE(so.get(), std::string{"test"});
  DRTEST_COMPARE(so.get("some slot 1"), std::string{"test 1"});
  DRTEST_COMPARE(so.get("some slot 2"), std::string{});
  so.set("some slot 2", "test 2");
  DRTEST_COMPARE(so.get(), std::string{"test"});
  DRTEST_COMPARE(so.get("some slot 1"), std::string{"test 1"});
  DRTEST_COMPARE(so.get("some slot 2"), std::string{"test 2"});
}
