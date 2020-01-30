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

#include "DrMock/Test.h"
#include "mock/RocketMock.h"
#include "LaunchPad.h"

DRTEST_TEST(launch)
{
  auto rocket = std::make_shared<drmock::samples::RocketMock>();

  // Define rocket's state behavior.
  rocket->mock.toggleLeftThruster().state()
      .transition("", "leftThrusterOn", true)
      .transition("leftThrusterOn", "", false)
      .transition("rightThrusterOn", "allThrustersOn", true)
      .transition("allThrustersOn", "rightThrusterOn", false);
  rocket->mock.toggleRightThruster().state()
      .transition("", "rightThrusterOn", true)
      .transition("rightThrusterOn", "", false)
      .transition("leftThrusterOn", "allThrustersOn", true)
      .transition("allThrustersOn", "leftThrusterOn", false);
  rocket->mock.launch().state()
      .transition("", "failure")
      .transition("*", "liftOff");

  // Run the test.
  drmock::samples::LaunchPad launch_pad{rocket};
  launch_pad.launch();
  DRTEST_ASSERT(rocket->mock.verifyState("liftOff"));
}
