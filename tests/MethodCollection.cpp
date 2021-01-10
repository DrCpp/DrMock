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

#include <DrMock/test/Test.h>
#include <DrMock/mock/MethodCollection.h>
#include <DrMock/mock/IMethod.h>

using namespace drmock;
using namespace drtest;

class MMethod final : public IMethod
{
public:
  MMethod(bool v): v_{v} {}
  bool verify() const override { return v_; }
  std::string makeFormattedErrorString() const override { return ""; }

private:
  bool v_ = false;
};

DRTEST_DATA(mainTest)
{
  addColumn<std::vector<std::shared_ptr<IMethod>>>("input");
  addColumn<bool>("result");

  addRow("true",
      std::vector<std::shared_ptr<IMethod>>{
          std::make_shared<MMethod>(true),
          std::make_shared<MMethod>(true),
          std::make_shared<MMethod>(true),
        },
      true
    );
  addRow("false",
      std::vector<std::shared_ptr<IMethod>>{
          std::make_shared<MMethod>(true),
          std::make_shared<MMethod>(false),
          std::make_shared<MMethod>(true),
        },
      false
    );
}

DRTEST_TEST(mainTest)
{
  DRTEST_FETCH(std::vector<std::shared_ptr<IMethod>>, input);
  DRTEST_FETCH(bool, result);

  MethodCollection collection{std::move(input)};
  DRTEST_COMPARE(collection.verify(), result);
}
