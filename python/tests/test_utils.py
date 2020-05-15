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

""" Makeshift test:

* utils.split
* utils.swap
"""

import unittest
from mocker import utils

class TestSplit(unittest.TestCase):
    def test_split(self):
        def first_char(s: str) -> str:
            return s[0]
        self.assertEqual(
            sorted(
                utils.split(
                    ["foo", "bar", "baz", "foobar", "barbar"],
                    first_char
                )
            ),
            sorted([
                ["foo", "foobar"],
                ["bar", "baz", "barbar"]
            ])
        )
        self.assertEqual(
            sorted(utils.split(
                [0, 3, 5, 2, -1, 2, 2, 3, 2],
                lambda i: (i == 2)
            )),
            sorted([
                [0, 3, 5, -1, 3],
                [2, 2, 2, 2]
            ])
        )


class TestSwap(unittest.TestCase):
    def test_failure(self):
        # No capture group in first argument.
        self.assertRaises(
            ValueError,
            utils.swap,
            "I.*", "\\1 ...", "IFoo"
        )
        # Multiple capture groups in first argument.
        self.assertRaises(
            ValueError,
            utils.swap,
            "(.*)([a-z])", "\\1 ...", "123a"
        )
        # No wildcard in second argument.
        self.assertRaises(
            ValueError,
            utils.swap,
            "I(.*)", "...", "IFoo"
        )
        # Multiple wildcards in second argument.
        self.assertRaises(
            ValueError,
            utils.swap,
            "I(.*)", "\\1...\\1", "IFoo"
        )
        # No match.
        self.assertRaises(
            ValueError,
            utils.swap,
            "I([A-Z].*)", "\\1bar", "Ifoo"
        )

    def test_success(self):
        self.assertEqual(
            utils.swap("I([A-Z].*)", "\\1Bar", "IFoo"),
            "FooBar"
        )
        self.assertEqual(
            utils.swap("\.\.\. (.*)", "\\1 ...", "... Foo"),
            "Foo ..."
        )
