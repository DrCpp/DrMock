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

import re

def split(li, f):
    """ Split `li` into sublists of equivalence classes.

    :example:
        
        r = split(["foo", "bar", "foobar"], len)
        r  # [["foo", "bar"], ["foobar"]]
    """
    values = {f(x) for x in li}
    return [[x for x in li if f(x) == value] for value in values]

def swap(regex: str, out: str, x: str) -> str:
    # Check that `regex` contains exactly one capture group.
    if re.compile(regex).groups != 1:
        raise ValueError(
            f"{regex} does not contain exactly one capture group"
        )
    # Check if `out` contains `"*"` exactly once as substring.
    if out.count("\\1") != 1:
        raise ValueError(
            f"{out} does not contain exactly one subexpression."
        )
    # Find a the capture group's match; raise if there is no match.
    match = re.match(regex, x)
    if not match:
        raise ValueError(
            f"{x} does not match {regex}."
        )
    inner = match.group(1)
    # Replace and return.
    z = out.replace("\\1", inner)
    return z
