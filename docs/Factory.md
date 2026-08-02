# Factory

The ```Factory``` class is responsible for constructing subclasses of a common base class from a common string identifier. 
It is intended to be used when loading serialized data from disk, allowing objects to be recreated without requiring 
knowledge of their concrete type. 

The factory itself does not know about any of the subclasses. Instead, each subclass is registered with a unique name. Once
registered, objects can be constructed by providing that name.

The ```Factory``` is a class template and can thereforce be used for any inheritance hierarchy. Typical uses include:

 - ```Device```
 - ```Object```

A factory stores constructor functions rather than objects themselves. Each constructor function is responsible for constructing
one concrete subclass.

Internally, each registration consists of:

 - A unique string identifier
 - A constructor function

## Concept

Conceptually a factory containing three registered classes looks like:

Identifier | Constructor
| :--- | :--- |
"Robot" | ```Robot(World&)``` | 
"Wall" | ```Wall(World&)``` | 
"Battery" | ```Battery(World&)``` | 

When an object is requested, the factory performs a lookup using the supplied identifier. If a matching constructor exists, the 
object is created and returned. Otherwise, ```nullptr``` is returned.

The factory owns no objects. Every call to ```create``` constructs a new instance.

## Registration

Types are registered using the ```registerType``` member function, an example is below:

```cpp
factory.registerType<Robot>("Robot");
factory.registerType<Wall>("Wall");
factory.registerType<Battery>("Battery");
```

Each registration associates a string identifier with a constructor function.

Only subclasses of the factory's base class may be registere. Attempting the register an unrelated type results in a compile-time
error.

Identifiers should be unique within a factory. Registering the same identifier multiple times is considered an error (and might cause
race conditions). All identifiers are turned into lowercases. This is not case sensitive. "Robot", "robot", and "roBoT" are all the
same according to the ```Factory```.

## Construction

Objects are constructed using the ```create``` function, an example is below:

```cpp
auto object = factory.create("Robot", world);
```

The factory locates the constructor associated with ```"Robot"``` and invokes it. This is equivalent to writing:

```cpp
Robot object = std::make_unique<Robot>(world);
```

but without the caller needing to know the concrete type beforehand.

If no matching registration exists, ```nullptr``` is returned.

## Constructor Requirements

Every registered class must be constructible using the argument list expected by the factor. 

For example a factory defined as:

```cpp
Factory<Object, World&>
```

requires every registered subclass to expose a constructor equivalent to:

```cpp
Derived<World&>;
```

A factory may be specialized with any constructor signature. Examples may include the following:

```cpp
Factory<Object, World&>

Factory<Device, World&, Robot&>

Factory<Component, World&, const Json&>
```

The factory will forward all supplied constructor arguments directly to the registered subclass. 

## Extending the Engine

The factory is designed to support user-defined subclasses. 

Applications may register their own object and device types alongside those provided by the engine, e.g.

```cpp
factory.registerType<MyLaser>("Laser");
factory.registerType<MySensor>("Sensor");
```

Once registered, custom classes participate in loading exaclty the same way as built-in engine classses. 
No modification to the engine source code is required. 

## Serialization

Factories are commonly used by serialization and deserialization systems. Rather than storing C++ type information,
serialized files store the registered identifiers. 

For example,

```
Robot
```

can later be deserialized by requesting:

```cpp
factory.create("Robot", world);
```

This separates the serialization format from implementation details while allowing new object types to be added 
without changing the loader.

## Design Notes

The factory stores constructor functions rather than instances.

This provides several advantages:

 - No object instances are allocated until requested
 - New subclasses can be registered without modifying existing code
 - Constructor logic is centralized
 - The serialization system only depends on registered identifiers
 - The factory is reusable for any inheritance history

The factory is intended for one-time registration during application startup. Once initialization is complete, 
registrations typically remain unchanged for the lifetime of the application.

### TODO

 - Make it so that factory expects lowercase only types (will convert to lowercase only so it is not case-sensitive)

### References

 - [Object](Object.md)
 - [Device](Device.md)
 - [World](World.md)
 - [Serialization](Serialization.md)
 - [Deserialization](Deserialization.md)