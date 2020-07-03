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

#ifndef DRMOCK_TESTS_DUMMY_H
#define DRMOCK_TESTS_DUMMY_H

#include <vector>
#include <string>

#include <QObject>

namespace drmock {

class Dummy : public QObject
{
  Q_OBJECT
public slots:
  void bazzify(std::string&);

signals:
  void pass_by_ref(std::string&);
  void multiple_params(int, float&, const std::vector<int>&);
  void no_params();
};

} // namespace drmock

#endif /* DRMOCK_TESTS_DUMMY_H */
