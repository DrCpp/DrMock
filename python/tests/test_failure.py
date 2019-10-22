# Copyright 2019 Ole Kliemann, Malte Kliemann
#
# This file is part of DrMock.
#
# DrMock is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# DrMock is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with DrMock.  If not, see <https://www.gnu.org/licenses/>.

""" Makeshift tests for:

* mocker.Method.from_cursor
* mocker.Type.from_spelling
"""

from mocker import parse
parse.configure("c++17")
import unittest

tests = {
    "TestFailureTrailing.h",
    "TestFailureSyntax.h",
    "TestFailureInclude.h"
}

class TestFailure(unittest.TestCase):
    def test_failure(self):
        pass
        # Check that the types are as expected.
        for filename in tests:
            with self.subTest(filename = filename):
                with open("resources/" + filename, "r") as file:
                    source = file.read()
                with self.assertRaises(RuntimeError):
                    parse.translate(source)
