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

#include "TestObject.h"

#include <sstream>

#include <DrMock/test/SkipTest.h>
#include <DrMock/test/TestFailure.h>
#include <DrMock/utility/ILogger.h>

namespace drtest { namespace detail {

namespace {

void log(
    const std::string& category,
    const std::string& test,
    const std::string& data,
    int line,
    const std::string& msg
  )
{
  std::string location{test};
  if (not data.empty())
  {
    location += ", " + data;
  }

  drutility::Singleton<drutility::ILogger>::get()->logMessage(
      false,
      category,
      location,
      line,
      std::stringstream{} << msg
   );
}

} // anonymous namespace

TestObject::TestObject(std::string name)
:
  name_{std::move(name)}
{}

void
TestObject::setTestFunc(std::function<void()> test_func)
{
  test_func_ = std::move(test_func);
}

void
TestObject::setDataFunc(std::function<void()> data_func)
{
  data_func_ = std::move(data_func);
}

void
TestObject::runOneTest(const std::string& row, bool verbose_logging)
{
  if (verbose_logging)
  {
    log("TEST", name_, row, -1, {});
  }
  if ((tags_[row] & tags::skip) == tags::skip)
  {
    log("SKIP", name_, row, -1, {});
    return;
  }
  try
  {
    test_func_();
  }
  catch(const SkipTest& e)
  {
    log("SKIP", name_, row, -1, {});
    return;
  }
  catch(const TestFailure& e)
  {
    if (xfail_ or ((tags_[row] & tags::xfail) == tags::xfail))
    {
      log("XFAIL", name_, row, e.line(), e.what());
    }
    else
    {
      log("*FAIL", name_, row, e.line(), e.what());
      failed_rows_.push_back(row);
    }
    return;
  }
  catch(const std::logic_error& e)
  {
    log("*FAIL", name_, row, -1, std::string{"logic_error: "} + std::string{e.what()});
    failed_rows_.push_back(row);
    return;
  }
  catch(const std::exception& e)
  {
    log("*FAIL", name_, row, -1, e.what());
    failed_rows_.push_back(row);
    return;
  }
  if (verbose_logging)
  {
    log("PASS", name_, row, -1, {});
  }
}

void
TestObject::prepareTestData()
{
  if (data_func_)
  {
    try
    {
      data_func_();
    }
    catch(const std::exception& e)
    {
      log("*ERROR", name_, "data", -1, e.what());
      failed_rows_.push_back("data");
    }
  }
}

void
TestObject::runTest(bool verbose_logging)
{
  failed_rows_.clear();
  if (data_rows_.size() > 0)
  {
    for (const auto& row : data_rows_)
    {
      current_row_ = row;
      runOneTest(row, verbose_logging);
    }
  }
  else
  {
    runOneTest({}, verbose_logging);
  }
}

std::size_t
TestObject::num_failures() const
{
  return failed_rows_.size();
}

void
TestObject::abs_tol(double value)
{
  abs_tol_ = value;
}

void
TestObject::rel_tol(double value)
{
  rel_tol_ = value;
}

void
TestObject::xfail()
{
  xfail_ = true;
}

void
TestObject::tagRow(const std::string& row, tags tag)
{
  tags_[row] |= tag;
}

}} // namespaces
