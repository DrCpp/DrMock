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

#ifndef DRMOCK_SAMPLES_MOCKING_SRC_WAREHOUSE_H
#define DRMOCK_SAMPLES_MOCKING_SRC_WAREHOUSE_H

#include <map>

#include "IWarehouse.h"

namespace drmock { namespace samples {

class Warehouse : public IWarehouse
{
public:
  void add(std::string commodity, std::size_t quantity) override final;
  bool remove(const std::string& commodity, std::size_t quantity) override final;

private:
  std::map<std::string, std::size_t> inventory_;
};

}} // namespace drmock::samples

#endif /* DRMOCK_SAMPLES_MOCKING_SRC_WAREHOUSE_H */
