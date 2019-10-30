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

This sample demonstrates **DrMock**'s state calculus and how to use it
for state verification.

# Table of contents

* [Introduction](#introduction)
* [State calculus](#state-calculus)
* [Testing states](#testing-states)
* [Running the tests](#running-the-tests)
* [Configuring state behavior](#configuring-state-behavior)
  + [transition](#transition)
  + [returns](#returns)
  + [throws](#throws)
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

## Introduction

Mocks are usually used to test objects against their implementation. For
example, in [samples/mock](#samplesmock), expecting the behavior
```
warehouse->mock.remove().push()
    .expect("foo", 2)
    .times(1)
    .returns(true);
```
only makes sense if `remove` is used in the implementation of `Order`.
This type of testing is called _behavior verification_ and is dependent
on the implementation of the system under testing.

To make tests less dependent on the implementation, **DrMock**'s state
calculus may be used. Consider the interface `IRocket`:
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
Thus, one would expect the implementation of `LaunchPad::launch()` to
be:
```cpp
void
LaunchPad::launch()
{
  rocket_->toggleLeftThruster(true);
  rocket_->launch();
}
```

But let's say the control room is full of frantic apes randomly bashing
the buttons, before (luckily!) enabling a thruster and then pressing
`rocket->launch()`:
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
the result should be testable. This is where **DrMock**'s state calculus
enters the stage!

## State calculus

Every mock object admits a private `StateObject`, which manages an
arbitrary number of _slots_, each of which has a _current state_. This
state object is shared between all methods of the mock object, but, per
default, it is not used. To enable a method `func` to use the shared
state object, do 
```cpp
foo->mock.func().state()
```
This call returns an instance of `StateBehavior`, which can be
configured in the same fashion as `Behavior`. 

Every slot of the `StateObject` is designated using an `std::string`, as
is every state. Upon execution of the test, the state of every slot is
the _default state_ `""`.

The primary method of controlling the state object is by defining
_transitions_. To add a transition to a method, do
```
rocket->mock.launch().state().transition(
    "main",
    "leftThrusterOn",
    "liftOff"
  );
```
This informs the state object to transition the slot `"main"` from the
state `"leftThrusterOn"` to `"liftOff"` when `launch()` is called.
If no slot is specified, as in
```
rocket->mock.launch().state().transition(
    "leftThrusterOn",
    "liftOff"
  );
```
then the _default slot_ `""` is used. 

**Note.** There is no need to add slots to the state object prior to
calling `transition`. This is done automatically. 

Now, `launch` doesn't take any arguments. If the underlying methods
takes arguments, the `transition` call requires an input. For example, 
```
rocket->mock.toggleLeftThruster().state().transition(
    "leftThrusterOn",
    "",
    false
  );
```
instructs the state object to transition the default slot from the state
`"leftThrusterOn"` to the default state `""` if
`toggleLeftThruster(false)` is called.

The wildcard symbol `"*"` may be used as catch-all for the current
state. Pushing regular transitions before or after a transition with
wilcard add exceptions to the catch-all:
```
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
      .transition("allThrusterOn", "rightThrusterOn", false);
  rocket->mock.launch().state()
      .transition("", "failure")
      .transition("*", "liftOff");
```
Recall that the state of every new slot is the default state `""`,
which, in this example, is used to model the `"allThrustersOff"` state.

After `launch()` is executed, the correctness of `launch()` is tested by
asserting that the current state of the default slot of `rocket` is
equal to `liftOff`:
```
DRTEST_ASSERT(rocket->mock.verifyState("liftOff");
```
(The method
`bool verifyState([const std::string& slot,] const std::string& state);` 
simply checks if the current state of `slot` is `state`.)

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

## Configuring state behavior

With the exception of `polymorphic`, the configuration methods take the
slot as optional first parameter. The default value is the default slot
`""`. 

The parameters `Result` and `Args...` are designators for the underlying
method's return value and parameters.

### transition

```
StateBehavior& transition(
   [const std::string& slot,]
    std::string current_state,
    std::string new_state,
    Args... input
  );
```
Instruct the `StateBehavior` to transition the slot `slot` from the state
`current_state` to `new_state` when the method is called with the
arguments `input...`.

### returns

```
StateBehavior& returns(
   [const std::string& slot,]
    const std::string& state,
    const Result& value
  );
StateBehavior& returns(
   [const std::string& slot,]
    const std::string& state,
    Result&& value
  );
```
Instruct the `StateBehavior` to return `value` if the state of `slot` is
`state`. `"*"` may not be used as catch-all in the `returns` method.

Example:
```
// ILever.h

class ILever
{
public:
  virtual void set(bool) = 0;
  virtual bool get() = 0;
};

// Some test...

DRTEST_TEST(...)
{
  auto lever = std::shared_ptr<LeverMock>();
  lever->mock.toggle().state()
      .transition("", "on", true)
      .transition("on", "", false);
  lever->mock.get().state()
      .returns("", false)
      .returns("on", true);
}
```

### throws

```
template<typename E> StateBehavior& throws(
   [const std::string& slot,]
    const std::string& state,
    E&& exception
  )
```
Instruct the `StateBehavior` to throw `exception` if the state of `slot`
is `state`.

### polymorphic

```
template<typename... Deriveds> StateBehavior& polymorphic();
```
Instruct the `StateBehavior` to expect as argument
`std::shared_ptr<Deriveds>...` or `std::unique_ptr<Deriveds>...`. 

See also: [Polymorphism](#polymorphism).

## State verification

Access to the mock object (except during configuration) can be entirely
eliminated in many cases, making the test entirely independent of the
implementation of the system under testing.

Consider the following example: 
```
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
This can be tested as follows:
```
DRTEST_TEST(open)
{
  // Configure mock.
  auto lever = std::make_shared<LeverMock>();
  lever->mock.toggle().state()
      .transition("", "on", true)
      .transition("on", "", false);
  lever->mock.get().state()
      .returns("", false)
      .returns("on", true);

  // Run the test.
  TrapDoor trap_door{lever};
  trap_door.toggle(true);
  DRTEST_ASSERT(trap_door.open());
}
```

Note that although the lever's behavior was configured prior to the
test, it was not verified after calling `toggle`. Only the trap door's
_state_ is verified using `DRTEST_ASSERT(trap_door.open());`. Thus,
`LeverMock` really served as a _stub_, using M. Fowler's terminology
[1].

## Bibliography

[1] [M. Fowler, _Mocks Aren't Stubs_](https://martinfowler.com/articles/mocksArentStubs.html)
