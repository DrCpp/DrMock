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

# Cookbook

The **DrMock** cookbook contains a collections of frequently required
patterns.

### Table of contents

* [Factories](#factories)
* [The QObject-Interface-Implementation pattern](#the-qobject-interface-implementation-pattern)

## Factories

Factories come into play when instances of a class `Foo` are dynamically created:

```cpp
// IFoo.h
class IFoo
{
public:
  virtual ~IFoo() = default;

  virtual int func() = 0;
  /* ... */
};

// Foo.h
class Foo : public IFoo
{
public:
  Foo(int)
  {
    /* ... */
  }

  /* ... */
};

// Bar.h
class Bar
{
public:
  void gunc()
  {
    int x = /* ... */;
    auto foo = std::make_shared<Foo>(x);
    /* Do something with `foo`... */
  }
};
```

Suppose we want to mock the role of the `Foo` instances in the method
`Bar::gunc`. The dependency injection *cannot* be done via templates.
Let's try:
```cpp
// Bar.h
template<typename T = Foo>
class Bar
{
public:
  void gunc()
  {
    int x = /* ... */;
    auto foo = std::make_shared<T>(x);
    /* Do something with `foo`... */
  }
};
```

In production code, we'd just use `Bar`, in test code, we could use
`Bar<FooMock>` to produce mock objects. But `FooMock` doesn't have a
ctor other than it's default, and there is not way to "reach" the
produced mock objects for configuration.

Instead, the dependency injections should happen via the factory
pattern:
```cpp
// AbstractFactory.h
template<typename Base, typename... Args>
class AbstractFactory
{
public:
  virtual ~AbstractFactory() = default;

  virtual std::shared_ptr<Base> make_shared(Args...) = 0;
};

// Factory.h
template<typename Base, typename Derived, typename... Args>
class Factory final : public AbstractFactory<Base, Args...>
{
public:
  std::shared_ptr<Base> make_shared(Args... args) override
  {
    return std::make_shared<Derived>(std::move(args)...);
  }
};

// Bar.h
class Bar
{
public:
  Bar()
  :
    Bar{std::make_shared<Factory<IFoo, Foo, int>>()}
  {}

  Bar(std::shared_ptr<AbstractFactory<IFoo, int>> factory)
  :
    factory_{std::move(factory)}
  {}

  void gunc()
  {
    int x = /* ... */;
    auto foo = factory_->make_shared(x);
    /* Do something with `foo`... */
  }

private:
  std::shared_ptr<AbstractFactory<IFoo, int>> factory_{};
};
```

By default, instances of `Bar` behave just as in the example above (as a
normal `Factory` is used by default), but in test code for the `Bar`
class, a dependency injection may be achieved by mocking
`AbstractFactory` and `IFoo` using **DrMock** and injecting a
`FactoryMock` into the system under test via the ctor
`Bar(std::shared_ptr<AbstractFactory<IFoo, int>>)`:

```cpp
DRTEST_TEST(factoryExample)
{
  auto factory = std::make_shared<FactoryMock<IFoo, int>>();

  // Create and configure output for `make_shared`:
  auto foo1 = std::make_shared<FooMock>();
  foo1->mock.func().returns(1).times(1);
  auto foo2 = std::make_shared<FooMock>();
  foo2->mock.func().returns(2).times(1);

  factory->mock.make_shared().state().transition("",       "state2",    1)
                                     .transition("state2", "exhausted", 2);
  factory->mock.make_shared().state().returns("",       foo1)
                                     .returns("state2", foo2);

  // Inject factory object into test object.
  Bar bar{factory};

  // Run the tests.
  bar.gunc();  // Factory produces `foo1` for internal use.
  bar.gunc();  // Factory produces `foo2` for internal use.
  /* ... */

  // Verify that `foo1` and `foo2` are used exactly once, as defined above.
  DRTEST_VERIFY_MOCK(foo1->mock.func());
  DRTEST_VERIFY_MOCK(foo2->mock.func());
}
```

Note in order to configure the produced instances of `FooMock`, it's
necessary to mock the factory itself. Also note that the assertion that
`foo1` and `foo2` are called exactly once implies the assertion that
`factory->make_shared()` was called twice, with the arguments `1` and
`2`.

## The QObject-Interface-Implementation pattern

Class templates may not be QObjects.
If you want to define a templated QObject interface,
you can't just do

```cpp
template<typename T>
class IFoo : public QObject
{
  Q_OBJECT

public:
  virtual ~IFoo() = default;

  virtual T f() = 0;  // Note: f depends on the template parameter!

public slots:
  virtual void slot() = 0;

signals:
  /* ... */
};

template<typename T>
class Foo : public Foo<T>
(
  /* ... */
};
```

or you'll run into trouble.
Instead, introduce an extra layer of abstraction:

```cpp
// FooQObject.h
class FooQObject : public QObject
{
  Q_OBJECT

public:
  virtual ~FooQObject() = default;

public slots:
  virtual void slot() = 0;

signals:
  /* ... */
};

// IFoo.h
template<typename T>
class IFoo : public FooQObject
{
public:
  virtual T f() = 0;

  virtual void slot() = 0;  // Required so that DrMock finds this function.
};

// Foo.h/Foo.cpp
template<typename T>
class Foo : public Foo<T>
{
public:
  T f() override
  {
    /* ... */
  }
  void slot() override
  {
    /* ... */
  }

  /* ... */
};
```

All slots and signals go into `FooQObject`.
Slots are pure virtual, signals are defined by the MOC.
It's prudent not to use `QFoo` instead of `FooQObject` to prevent
collisions with the Qt namespace.

The other pure virtual methods,
even those whose signature doesn't contain any template parameter,
are declared in `IFoo`,
_alongside duplicates of all public slots_
(note the occurence of `slot` in `IFoo`).
This is required so that all pure virtual methods
are declared in the interface that **DrMock** receives.
If you fail to do so, **DrMock** will not implement the slots,
`FooMock` will be abstract instead of an implementation of `IFoo`,
and the test code will most likely throw a compilation error similar to this one:

```cpp
error: abstract class is marked 'final' [-Werror,-Wabstract-final-class] class FooMock final : public IFoo<T>
```
