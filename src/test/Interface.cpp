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

#include "Interface.h"

#include "SkipTest.h"

namespace drtest {

void
abs_tol(double value)
{
  drutility::Singleton<detail::Global>::get()->abs_tol(value);
}

void
rel_tol(double value)
{
  drutility::Singleton<detail::Global>::get()->rel_tol(value);
}

void
tagRow(const std::string& row, tags tag)
{
  drutility::Singleton<detail::Global>::get()->tagRow(row, tag);
}

void
skip()
{
  skip("");
}

void
skip(std::string what)
{
  throw SkipTest{what};
}

void
xfail()
{
  drutility::Singleton<detail::Global>::get()->xfail();
}

} // namespace drtest
