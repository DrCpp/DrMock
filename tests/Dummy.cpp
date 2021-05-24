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

#include "Dummy.h"

namespace drmock {

Dummy::Dummy(Qt::ConnectionType connection_type)
{
  QObject::connect(
      this, &Dummy::no_params,
      this, &Dummy::no_params_slot,
      connection_type
    );
  QObject::connect(
      this, &Dummy::multiple_params,
      this, &Dummy::multiple_params_slot,
      connection_type
    );
  QObject::connect(
      this, &Dummy::pass_by_ref,
      this, &Dummy::pass_by_ref_slot,
      connection_type
    );
}

unsigned int
Dummy::no_params_count() const
{
  return no_params_count_;
}

std::tuple<int, const QString*>
Dummy::multiple_params_value() const
{
  return multiple_params_;
}

QString*
Dummy::pass_by_ref_value() const
{
  return pass_by_ref_;
}

void
Dummy::no_params_slot()
{
  no_params_count_++;
}

void
Dummy::multiple_params_slot(int x, const QString& y)
{
  multiple_params_ = std::make_tuple(x, &y);
}

void
Dummy::pass_by_ref_slot(QString& s)
{
  pass_by_ref_ = &s;
}

} // namespace drmock
