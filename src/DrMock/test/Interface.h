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

#ifndef DRMOCK_SRC_DRMOCK_TEST_INTERFACE_H
#define DRMOCK_SRC_DRMOCK_TEST_INTERFACE_H

#include <string>

#include <DrMock/test/Tags.h>

namespace drtest {

namespace detail {

template<typename T1, typename T2>
using Replace = T2;

} // namespace detail

template<typename T> void addColumn(std::string);
template<typename... Ts> void addColumns(detail::Replace<Ts, std::string>...);
template<typename... Ts> void addRow(const std::string& row, Ts&&... ts);
template<typename T> bool almostEqual(T actual, T expected);
void abs_tol(double value);
void rel_tol(double value);
void tagRow(const std::string& row, tags tag);
void skip();
void skip(std::string what);
void xfail();

} // namespace drtest

#include "Interface.tpp"

#endif /* DRMOCK_SRC_DRMOCK_TEST_INTERFACE_H */
