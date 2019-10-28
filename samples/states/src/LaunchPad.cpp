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

#include "LaunchPad.h"

#include <random>

namespace drmock { namespace samples {

LaunchPad::LaunchPad(std::shared_ptr<IRocket> rocket)
:
  rocket_{std::move(rocket)}
{}

void
LaunchPad::launch()
{
  // Randomly toggle the thrusters.
  std::random_device rd{};
  std::mt19937 gen{rd()};
  std::bernoulli_distribution dist{0.5};
  for (std::size_t i = 0; i < 19; i++)
  {
    rocket_->toggleLeftThruster(dist(gen));
    rocket_->toggleRightThruster(dist(gen));
  }

  // Toggle at least one thruster and engage!
  rocket_->toggleLeftThruster(true);
  rocket_->launch();
}

}} // namespace drmock::samples
