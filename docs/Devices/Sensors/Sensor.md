# Sensor

`Sensor.hpp` defines the base `Sensor` device used by simulated robots.

## Sensor

`Sensor` publicly inherits from `Device` and provides the common interface for devices that measure the simulated environment.

Every concrete sensor implements:

```cpp
virtual void sense() = 0;
```

`Device::update()` is overridden so that updating a sensor performs a measurement.

## Responsibilities

`Sensor` provides the common sensor interface but does not define how a measurement is performed.

Concrete sensors are responsible for determining:

* What they measure.
* How they obtain the measurement.
* How the measurement is stored or exposed.
* Any sensor-specific configuration.

For example, a future distance sensor could perform a raycast and store the resulting distance.

## Design

The base class intentionally contains very little functionality. World queries such as raycasting belong to the `World` rather than to `Sensor`, allowing sensors to remain independent of the world's internal collision implementation.

Future sensor-specific functionality such as update rates, noise models, and filtering should be added only when there is a concrete need for it.

## References

* [Device](../../Device.md)
* [World](../../World.md)