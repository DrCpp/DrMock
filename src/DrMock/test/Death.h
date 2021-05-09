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

#ifndef DRMOCK_SRC_DRMOCK_TEST_DEATH_H
#define DRMOCK_SRC_DRMOCK_TEST_DEATH_H

#include <csignal>

#if defined(__unix__) || defined(__APPLE__)
#include <unistd.h>
#endif

#include <DrMock/test/TestFailure.h>

namespace drtest { namespace death {

static int pipe_[2];
volatile std::sig_atomic_t atomic_pipe_;  // Self-pipe write end; required due to https://en.cppreference.com/w/c/program/signal

#if defined(__unix__) || defined(__APPLE__)
static std::vector<int> signals_ = {  // POSIX signals, taken from https://man7.org/linux/man-pages/man7/signal.7.html
    SIGABRT,
    SIGALRM,
    SIGBUS,
    SIGCHLD,
    SIGCONT,
    SIGFPE,
    SIGHUP,
    SIGILL,
    SIGINT,
    // SIGKILL,
    SIGPIPE,
    SIGPROF,
    SIGQUIT,
    SIGSEGV,
    // SIGSTOP,
    SIGTSTP,
    SIGSYS,
    SIGTERM,
    SIGTRAP,
    SIGTTIN,
    SIGTTOU,
    SIGURG,
    // SIGUSR1,
    SIGUSR2,
    SIGVTALRM,
    SIGXCPU,
    SIGXFSZ
  };
#endif

// Signal handler requires external linkage according to https://en.cppreference.com/w/c/program/signal
extern "C" {
  void signal_handler(int x)
  {
    write(atomic_pipe_, &x, 4);
    exit(0);
  }
} // extern C

}} // namespace drtest::death

#define DRTEST_ASSERT_DEATH(statement, expected) \
do \
{ \
  pipe(drtest::death::pipe_); \
  drtest::death::atomic_pipe_ = drtest::death::pipe_[1]; \
\
  pid_t pid = fork(); \
  if (pid == 0) \
  { \
    for (auto s: drtest::death::signals_) \
    { \
      std::signal(s, drtest::death::signal_handler); \
    } \
    statement; /* Child exits here if signal is raised. */ \
    int no_signal = -1; \
    write(drtest::death::atomic_pipe_, &no_signal, 4); /* Wake up parent if no signal was raised. */ \
    close(drtest::death::pipe_[0]); \
    close(drtest::death::pipe_[1]); \
    exit(0); \
  } \
  else \
  { \
    assert(PIPE_BUF >= 4); \
    std::vector<char> buffer(4); \
    if (!read(drtest::death::pipe_[0], buffer.data(), 4)) \
    { \
      throw std::runtime_error{"read to pipe failed"}; \
    } \
\
    int result = *(int*)buffer.data(); \
    if (result != expected) \
    { \
\
      /* Error message */ \
      std::string e = strsignal(expected); \
      std::string r; \
      if (result != -1) \
      { \
        r = strsignal(result); \
      } \
      else \
      { \
        r = "No signal: -1"; \
      } \
      throw drtest::detail::TestFailure{__LINE__, "!=", "received", "expected", e, r}; \
    } \
  } \
\
  close(drtest::death::pipe_[0]); \
  close(drtest::death::pipe_[1]); \
} while(false)

#endif /* DRMOCK_SRC_DRMOCK_TEST_DEATH_H */
