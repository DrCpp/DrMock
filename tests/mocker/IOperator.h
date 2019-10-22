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

#ifndef DRMOCK_TESTS_MOCKER_IOPERATOR_H
#define DRMOCK_TESTS_MOCKER_IOPERATOR_H

#include <vector>

namespace outer { namespace inner {

class IOperator
{
public:
  virtual ~IOperator() = default;

  virtual bool operator            ==(const int& x) = 0;
  virtual void operator +(std::vector<int>& x) = 0;
  virtual int operator( )(int, float, double* const) = 0;
};

}} // namespace outer::inner

#endif /* DRMOCK_TESTS_MOCKER_IOPERATOR_H */
