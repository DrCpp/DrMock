<!--
Copyright 2020 Ole Kliemann, Malte Kliemann

This file is part of DrMock.

DrMock is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

DrMock is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with DrMock.  If not, see <https://www.gnu.org/licenses/>.
-->

# samples/death

DrMock 0.4.0 introduces death tests. A _death test_ checks if a certain
statement will cause the process to raise a certain signal. This may be
used to assert that in certain unrecoverable situations, the program
exits before causing further damage.

This sample demonstrates the death testing capabilities of **DrMock**.

### Table of contents

* [Source code](#source-code)
* [Running the tests](#running-the-tests)
* [Details](#details)
  + [Supported signals](#supported-signals)
  + [clone, fork, signal, multi-threading](#clone-fork-signal-multi-threading)

### Project structure

```
samples/death
│   CMakeLists.txt
│   Makefile
│   deathTest.cpp
```

### Requirements

This project requires an installation of **DrMock** in `build/install/`
or a path container in the `CMAKE_PREFIX_PATH` variable. If your
installation of **DrMock** is located elsewhere, you must change the
value of `CMAKE_PREFIX_PATH`.

## Source code

Open `deathTest.cpp`!
The `DRTEST_ASSERT_DEATH(statement, expected)` macro checks if executing
`statement` will cause the signal `expected` to be raised. As with
`DRTEST_ASSERT_THROW`, statement may contain multiple lines of code, if
they are seperated by semicolons (see below).

In the test `catch_segfault`, we test the classic segmentation fault
scenario, dereferencing a `nullptr`:
```cpp
DRTEST_TEST(catch_segfault)
{
  DRTEST_ASSERT_DEATH(
      int* foo = nullptr;
      *foo = 0,
      SIGSEGV
    );
}
```
We expect this to raise the `SIGSEGV` signal. The test will verify this.

## Running the tests

Do `make`. This should yield the following:

```
    Start 1: deathTest
1/1 Test #1: deathTest ........................   Passed    0.00 sec

100% tests passed, 0 tests failed out of 1

Total Test time (real) =   0.01 sec
```

## Details

### Supported signals

The following POSIX signals may be caught using `DRTEST_ASSERT_DEATH`:
```cpp
SIGABRT, SIGALRM,   SIGBUS,  SIGCHLD,
SIGCONT, SIGFPE,    SIGHUP,  SIGILL,
SIGINT,  SIGPIPE,   SIGPROF, SIGQUIT,
SIGSEGV, SIGTSTP,   SIGSYS,  SIGTERM,
SIGTRAP, SIGTTIN,   SIGTTOU, SIGURG,
SIGUSR2, SIGVTALRM, SIGXCPU, SIGXFSZ
```
Note that `SIGKILL`, `SIGSTOP` and `SIGUSR1` are not supported.

### clone, fork, signal, multi-threading

Using `clone()`, `fork()`, `signal()` or multi-threading are not allowed
when using `DRTEST_ASSERT_DEATH`.

### Log messages

Although `DRTEST_ASSERT_DEATH` may be used to catch `SIGABRT` raised by
a failed assertion, it will not catch log messages sent by the `assert`
macro. Depending on the implementation, even on a successful
`DRTEST_ASSERT_DEATH`, you may receive a log message of the following
kind:

```cpp
Assertion failed: (false), function death_success, file /path/to/test, line 213.
```
