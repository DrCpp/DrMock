<!--
Copyright 2019 Ole Kliemann, Malte Kliemann

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

# samples/states

This sample demonstrates how to use **DrMock** for state verification.

# Table of contents

* [Introduction](#introduction)
* [State calculus](#state-calculus)
* [Testing states](#testing-states)
* [Running the tests](#running-the-tests)
* [Configuring state behavior](#configuring-state-behavior)
  + [transition](#transition)
  + [returns](#returns)
  + [throws](#throws)
  + [emits](#emits)
  + [polymorphic](#polymorphic)
* [State verification](#state-verification)
* [Bibliography](#bibliography)

### Project structure

```
samples/states
│   CMakeLists.txt
│   Makefile
│
└───src
│   │   CMakeLists.txt
│   │   IRocket.h
│   │   LaunchPad.cpp
│   │   LaunchPad.h
│
└───tests
    │   CMakeLists.txt
    │   LaunchPad.cpp
```

### Requirements

This project requires an installation of **DrMock** in `build/install/`
or a path container in the `CMAKE_PREFIX_PATH` variable. If your
installation of **DrMock** is located elsewhere, you must change the
value of `CMAKE_PREFIX_PATH`.

## Introduction

Mocks are usually used to test _specific_ implementations of an interface.
For example, in [samples/mock](#samplesmock), expecting the behavior
```cpp
warehouse->mock.remove().push()
    .expect("foo", 2)
    .times(1)
    .returns(true);
```
only makes sense if `remove` is used in the implementation of `Order`.
This type of testing is called _behavior verification_ and is dependent
on the implementation of the system under test [1].

To make tests less dependent on the implementation, **DrMock**'s
state-machine mock objects may be used. Consider the interface
`IRocket`:
```cpp
// IRocket.h

class IRocket
{
public:
  virtual ~IRocket() = default;

  virtual void toggleLeftThruster(bool) = 0;
  virtual void toggleRightThruster(bool) = 0;
  virtual void launch() = 0;
};
```
A rocket may be launched if at least one of its thrusters is toggled.
The `LaunchPad` is responsible for enabling at least one thruster before
launching the rocket:
```cpp
// LaunchPad.h

class LaunchPad
{
public:
  LaunchPad(std::shared_ptr<IRocket>);

  void launch();

private:
  std::shared_ptr<IRocket> rocket_;
};
```
What would you expect the implementation of `LaunchPad::launch()` to be?
Should the `LaunchPad` enable only one thruster (which one?), or both.
But this is still fine, as we could verify if at least one thruster was
activated. But what about the following:
```cpp
void
LaunchPad::launch()
{
  rocket_->toggleLeftThruster(true);
  rocket_->toggleLeftThruster(false);
  rocket_->toggleLeftThruster(true);
  rocket_->launch();
}
```
This is a questionable, but correct implementation of `launch()`. But
this cannot be tested without tracking the _state_ of the thrusters.

It could be even worse.
For the sake of demonstration, let's say the control room is full of
frantic apes randomly bashing the buttons, before (luckily!) enabling a
thruster and then pressing `rocket->launch()`:
```cpp
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
```
There is no way to predict the behavior of `LaunchPad::launch()`. Yet,
the result should be testable. This is where **DrMock**'s state behavior
enters the stage!


## State behavior

The controller of every mock objects
admits a private `StateObject` (the "state object"), which manages an
arbitrary number of _slots_, each of which has a _current state_. This
state object is shared between all methods of the mock object, but, per
default, it is not used.

To disable the `BehaviorQueue` of the mock object and enable the
`StateBehavior` (the "state behavior") which makes use of the
state object, run
```cpp
foo->mock.func().state()
```
This call returns a `StateBehavior&`, which can be
configured in similar fashion to `BehaviorQueue`.

Every slot of the state object is designated using an `std::string`, as
is every state. The default state after initialization of every slot is
the _default state_ `""`.

The primary method of controlling the state object is by defining
_transitions_. To add a transition to a method, do
```cpp
rocket->mock.launch().state()
    .transition(
        "main",
        "leftThrusterOn",
        "liftOff"
      );
```
This informs the state object to transition the slot `"main"` from the
state `"leftThrusterOn"` to `"liftOff"` when `launch()` is called.
If no slot is specified, as in
```cpp
rocket->mock.launch().state()
    .transition(
        "leftThrusterOn",
        "liftOff"
      );
```
then the _default slot_ `""` is used.

*Note.* There is no need to add slots to the state object prior to
calling `transition`. This is done automatically.

If the underlying methods takes arguments, the `transition` call
requires an input. For example, 
```cpp
rocket->mock.toggleLeftThruster().state()
    .transition(
        "leftThrusterOn",
        "",
        false
      );
```
instructs the state object to transition the default slot from the state
`"leftThrusterOn"` to the default state `""` if
`toggleLeftThruster(false)` is called. We will describe the API of
`transition()` in detail below.

The wildcard symbol `"*"` may be used as catch-all/fallthrough symbol
for the current state. Pushing regular transitions before or after a
transition with wilcard add exceptions to the catch-all:
```cpp
rocket->mock.launch().state()
    .transition("*", "liftOff")
    .transition("", "failure");
```
If `launch()` is called, the default slots transitions to `"liftOff"`
from any state except the default state, which transitions to
`"failure"`.

## Testing states

As usual, the mock's behavior is configured at the start of the test:
Liftoff can only succeed if at least one thruster is on.
```cpp
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
```
Recall that the state of every new slot is the default state `""`,
which, in this example, is used to model the `"allThrustersOff"` state.

After `launch()` is executed, the correctness of `launch()` is tested by
asserting that the current state of the default slot of `rocket` is
equal to `liftOff`:
```cpp
DRTEST_ASSERT(rocket->mock.verifyState("liftOff");
```

(The method
`bool verifyState([const std::string& slot,] const std::string& state)`
checks if the current state of `slot` is `state`.)

Thus, except for the configuration calls and the singular call to
`verifyState`, no access or knowledge of the implementation was required
to test `LaunchPad::launch()`. As demonstrated in
[Using DrMock for state verification](#using-drmock-for-state-verification),
one can sometimes even do better.

## Running the tests

Running the test should produce:
```
    Start 1: LaunchPadTest
1/1 Test #1: LaunchPadTest ....................   Passed    0.00 sec

100% tests passed, 0 tests failed out of 1

Total Test time (real) =   0.01 sec
```
Feel free to rerun this test until you're convinced that the random
numbers generated in the implementation of `LaunchPad::launch()` have no
effect.

## `StateBehavior` API

Before going into detail, you should be familiar with the
`Behavior`/`BehaviorQueue` API from the previous chapter.

Unlinke `Behavior` objects, a `StateBehavior` does not expect any input.
Instead, it reacts to calls by changing ("transitioning") the states of
the state object. You've already seen examples of this:
```cpp
rocket->mock.toggleLeftThruster().state()
    .transition(
        "leftThrusterOn",
        "",
        false
      );
```
(Switching off the left thruster when the state is "leftThrusterOn"
returns the default slot to the default state, `""`.)

To handle returns, throws and emits, the state behavoir declares one of
the slots the _result slot_. How the result slot is selected is detailed
below. Once set, the result slot cannot be changed.

With the exception of `polymorphic`, the configuration methods take the
slot as optional first parameter. The default value is the default slot
`""`. A comprehensive summary of the `StateBehavior` API (optional
parameters marked with `[]` plus default value) follows:

```cpp
StateBehavior& transition(
    [const std::string& slot = "",]
    const std::string& current_state,
    std::string new_state,
    detail::Expect<Args>... input
  )
```

Add a transition: _If `slot` has current state `current_state` and the
method is called with `input...`, then change the state of `slot` to
`new_state`_. As in the case of `Behavior`, each element of `input...`
may be specified as raw input or as matcher.

```cpp
template<typename T = ReturnType> StateBehavior& returns(
    [const std::string& slot = "",]
    const std::string& state,
    std::enable_if_t<not std::is_same_v<ReturnType, void>, T>&& value
  );
```

Set a return value for a slot/state combination.

`slot` must be the result slot. If no result slot is set when `returns`
is called, `slot` is defined as the result slot.

```cpp
template<typename E> StateBehavior& throws(
    [const std::string& slot = "",]
    const std::string& state,
    E&& excp
  );
```
Throw on the provided slot/state combination.
`slot` must be the result slot. If no result slot is set when `throws`
is called, `slot` is defined as the result slot.

```cpp
template<typename... SigArgs> StateBehavior& emits(
    const std::string& state,
    const std::string& slot,
    void (Class::*signal)(SigArgs...),
    SigArgs&&... args
  );
```
Emit a Qt signal on the provided slot/state combination.
`slot` must be the result slot. If no result slot is set when `emits`
is called, `slot` is defined as the result slot.

```cpp
template<typename Deriveds...> StateBehavior& polymorphic()
```
Change the derived type of the matching handler.

Using the wildcard state `"*"` for the `current_state` parameter results
in the configuration serving as catch-all (or fallthru), to which other
configuration calls act as exceptions (as described above).

When the underlying method (i.e. the `Method` object) is called, then
the state behavior _first_ transitions _all_ of its slots according the
transitions recorded by the user. _Then_ it returns the result (return
and/or emit _or_ throw), which is executed by the method.

*Note.* Beware of inconsistencies in the transition table. It is
possible that `(current_state, input...)` matches multiple entries of
the transition table with the same slot (this depends on the `matcher`).
If this is the case, the transition that is executed is undefined.


## State verification

Access to the mock object (except during configuration) can be entirely
eliminated in many cases, thus freeing the programmer to have any
knowledge of the implementation of the system under test.

Consider the following example: 
```cpp
class ILever
{
public:
  virtual void set(bool) = 0;
  virtual bool get() = 0;
};

class TrapDoor
{
public:
  TrapDoor(std::shared_ptr<ILever>);

  bool open()
  {
    return lever_.get();
  }
  void toggle(bool v)
  {
    lever_.set(v);
  }

private:
  std::shared_ptr<ILever> lever_;
};
```

Usually, verifying correctness of `TrapDoor` would look something like
this:
```cpp
DRTEST_TEST(toggle)
{
  // Configure mock.
  auto lever = std::make_shared<LeverMock>();
  lever->mock.toggle().expect(true);

  // Configure SUT.
  TrapDoor trap_door{lever};

  // Run the test.
  trap_door.toggle(true);
  DRTEST_VERIFY_MOCK(lever->mock);
}
```
In other words: Call `trap_door.toggle(...)` and verify that `lever`
behaves as expected. This is good old behavior verification.

But you can also do this:
```cpp
DRTEST_TEST(interactionOpenToggle)
{
  // Configure mock.
  auto lever = std::make_shared<LeverMock>();
  lever->mock.toggle().state()
      .transition("", "on", true)
      .transition("on", "", false);
  lever->mock.get().state()
      .returns("", false)
      .returns("on", true);

  // Configure SUT.
  TrapDoor trap_door{lever};

  // Run the test.
  trap_door.toggle(true);
  DRTEST_ASSERT(trap_door.open());
}
```
Note that although the lever's behavior was configured prior to the
test, it was not verified after calling `toggle`. Instead of testing the
behavior of each method of `TrapDoor` using mocks, the _interaction_
between `TrapDoor`'s methods is tested (the door is `open()` after
`toggle(true)`, etc.). Only the trap door's _state_ is verified using
`DRTEST_ASSERT(trap_door.open());`. This requires no knowledge of the
implementation, only the interface and the specified behavior. This is
essentially what's called _state verification_, and in this context
`lever` would be refered to as a _stub_, not a mock.

For more on mocks and stubs, see [1].

## Bibliography

[1] [M. Fowler, _Mocks Aren't Stubs_](https://martinfowler.com/articles/mocksArentStubs.html)
