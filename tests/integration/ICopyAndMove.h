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

#ifndef DRMOCK_TESTS_INTEGRATION_COPYANDMOVE_H
#define DRMOCK_TESTS_INTEGRATION_COPYANDMOVE_H

namespace outer { namespace inner {

class NotCopyConstructible
{
public:
  NotCopyConstructible(int id) : id_{id} {}
  NotCopyConstructible(const NotCopyConstructible&) = delete;
  NotCopyConstructible& operator=(const NotCopyConstructible&) const = delete;
  NotCopyConstructible(NotCopyConstructible&&) = default;
  NotCopyConstructible& operator=(NotCopyConstructible&&) = default;

  bool operator==(const NotCopyConstructible& other) const
  {
    return (id_ == other.id_);
  }

  int id() const
  {
    return id_;
  }

private:
  int id_;
};

class ICopyAndMove
{
public:
  virtual ~ICopyAndMove() = default;

  virtual void byValue(outer::inner::NotCopyConstructible) = 0;
  virtual void lValueRef(const outer::inner::NotCopyConstructible&) = 0;
  virtual void rValueRef(outer::inner::NotCopyConstructible&&) = 0;
};

}} // namespace outer::inner

#endif /* DRMOCK_TESTS_INTEGRATION_COPYANDMOVE_H */
