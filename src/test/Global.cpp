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

#include "Global.h"

#include <sstream>

#include "utility/ILogger.h"
#include "utility/Singleton.tpp"

template class drutility::Singleton<drtest::detail::Global>;

namespace drtest { namespace detail {

Global::Global()
:
  reserved_names_{
      "initTestCase",
      "cleanupTestCase",
      "init",
      "cleanup"
    }
{
  addTestFunc("initTestCase", [] () {});
  addTestFunc("cleanupTestCase", [] () {});
  addTestFunc("init", [] () {});
  addTestFunc("cleanup", [] () {});
}

void
Global::addTest(std::string test_name)
{
  if (tests_.find(test_name) == tests_.end())
  {
    tests_.insert({test_name, TestObject{test_name}});
    test_names_.push_back(std::move(test_name));
  }
}

void
Global::addTestFunc(const std::string& test_name, std::function<void()> test_func)
{
  addTest(test_name);
  tests_[test_name].setTestFunc(std::move(test_func));
}

void
Global::addDataFunc(const std::string& test_name, std::function<void()> data_func)
{
  addTest(test_name);
  tests_[test_name].setDataFunc(std::move(data_func));
}

void
Global::runTests()
{
  TestObject& initTestCase = tests_["initTestCase"];
  TestObject& cleanupTestCase = tests_["cleanupTestCase"];

  initTestCase.runTest(false);
  if (initTestCase.num_failures() != 0)
  {
    return;
  }

  for (auto& test_name : test_names_)
  {
    if (reserved_names_.find(test_name) != reserved_names_.end())
    {
      continue;
    }

    current_test_ = test_name;

    TestObject& init = tests_["init"];
    TestObject& test = tests_[test_name];
    TestObject& cleanup = tests_["cleanup"];

    init.runTest(false);
    if (init.num_failures() != 0)
    {
      return;
    }

    test.prepareTestData();
    if (test.num_failures() != 0)
    {
      return;
    }
    test.runTest(true);

    cleanup.runTest(false);
    if (init.num_failures() != 0)
    {
      return;
    }
  }

  cleanupTestCase.runTest(false);
}

std::size_t
Global::num_failures() const
{
  std::size_t result = 0;
  for (auto& test : tests_)
  {
    result += std::get<TestObject>(test).num_failures();
  }
  return result;
}

void
Global::runTestsAndLog()
{
  runTests();

  drutility::Singleton<drutility::ILogger>::get()->logMessage(
      false,
      "",
      "",
      -1,
      std::stringstream{} << "****************"
   );
  std::size_t failed = num_failures();
  if (failed == 0)
  {
    drutility::Singleton<drutility::ILogger>::get()->logMessage(
        false,
        "",
        "",
        -1,
        std::stringstream{} << "ALL PASS"
      );
  }
  else
  {
    drutility::Singleton<drutility::ILogger>::get()->logMessage(
        false,
        "",
        "",
        -1,
        std::stringstream{} << failed << " FAILED"
     );
  }
}

void
Global::mark(Mark m)
{
  tests_[current_test_].mark(m);
}

}} // namespaces
