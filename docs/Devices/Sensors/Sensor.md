# Sensor

`Sensor.hpp` defines the base `Sensor` device used by simulated robots, along with the `ViewSensor` implementation.

## Sensor

`Sensor` publicly inherits from `Device` and provides the common interface and configuration shared by simulated sensors.

A `Sensor` consists primarily of:

* A `SensorShape` defining the region of the world that the sensor can inspect.
* A local `Transform` defining the sensor's position and orientation relative to its owning object.
* A sensor-specific measurement produced by a concrete sensor implementation.

`Sensor` is an abstract class. It does not define what a sensor measures or how the measurement is produced.

## Sensor Shape

Each sensor owns a `SensorShape` through a `std::unique_ptr`.

The shape defines the geometric region that the sensor uses when determining which positions in the world are within its sensing area.

The shape is independent of the sensor's local transform. A sensor can therefore combine a geometric shape with its own position and orientation.

The available shape types are:

* `SensorShapeQuadratic` — a four-vertex polygon.
* `SensorShapeCone` — a directional cone defined by a field of view and range.
* `SensorShapeBall` — a circular region defined by a radius.

See [Sensor Shape](SensorShape.md) for more information.

## Local Transform

`localTransform_` describes the sensor's position and orientation relative to the object to which the sensor is attached.

The sensor's world transform is therefore derived from the owning object's transform and the sensor's local transform.

For example, a camera mounted on the front of a robot can have a local position several units in front of the robot and a local rotation corresponding to the direction in which the camera faces.

The local transform does not itself represent the sensor's position in world coordinates.

## Sensing

`Sensor` declares the pure virtual function:

```cpp
virtual void sense() = 0;
```

Concrete sensors implement this function to perform their specific measurement.

The base class intentionally does not define the result of `sense()`. Different sensors may produce fundamentally different types of measurements.

For example:

* A view sensor can produce an `Image`.
* A distance sensor could produce a distance measurement.
* A temperature sensor could produce a temperature value.
* A ray sensor could produce information about the first object intersected by a ray.

The current interface has `sense()` return `void`. The resulting measurement is instead stored by the concrete sensor and exposed through that sensor's interface.

## Updating

`Sensor` overrides `Device::update()`.

The update function provides the normal device lifecycle through which a sensor can update its internal state based on the simulation's elapsed time.

The implementation of `update()` determines when sensing occurs. A sensor does not necessarily need to perform a measurement on every simulation update; this allows future implementations to support sensor-specific update rates or other timing behaviour.

## ViewSensor

`ViewSensor` is a concrete `Sensor` designed to represent a visual sensor, such as a camera.

It uses a `SensorShape` to determine the region of the world visible to the sensor. Its primary configuration consists of:

* Field of view (`fov`).
* Detection range (`range`).
* Local transform.

The convenience constructor accepting `fov` and `range` creates a view sensor using a cone-shaped sensing region.

## World Transform

`ViewSensor::worldTransform()` calculates the sensor's global transform from its owning object's transform and its local transform.

This is necessary because the sensor's local position and orientation move with the robot or object to which the sensor is attached.

The world transform represents where the sensor actually exists in the simulated world at the current point in time.

## Image

`ViewSensor` stores its most recent measurement in an `Image`.

Calling `sense()` updates this stored image using the environment visible from the sensor's current world transform and sensing shape.

The most recently produced image can be accessed through:

```cpp
const Image& image() const noexcept;
```

The returned image is read-only, preventing callers from modifying the sensor's stored measurement through this interface.

## Serialization

Both `Sensor` and concrete sensor classes support serialization and deserialization through `nlohmann::json`.

The serialized representation contains the sensor's configuration, including its shape and local transform, while concrete sensor types may add their own configuration.

Sensor construction from serialized data is performed through the project's factory mechanism where applicable.

## Design Responsibilities

`Sensor` is responsible for:

* Owning the sensor's sensing shape.
* Storing the sensor's local transform.
* Providing the common sensor interface.
* Integrating sensors with the `Device` update lifecycle.
* Providing serialization and deserialization support.

`Sensor` is **not** responsible for implementing the world's collision or spatial-query system.

World-level operations should remain within `World`. A sensor describes **what and where it wants to sense**, while the world provides the simulation data required to perform that query.

This separation prevents sensors from becoming tightly coupled to the internal representation of the simulated world.

## References

* [Device](../../Device.md)
* [World](../../World.md)
* [Sensor Shape](SensorShape.md)
* [Image](Image.md)
