# Image

The `Image` class represents a list of `Detection`s. `Image` allows access to its vector
of `Detection`s. `Image` simply encapsulates these `Detection`s.

## Capabilities

`Image`'s functions consist of:

* `detections`: returns the list of `Detection`s
* `empty`: checks whether its list of `Detection`s is empty
* `size`: checks the size of its list of `Detection`s

# Detection

The `Detection` structure encapsulates which `Object` was detected, and where. It simply
consists of:

* `std::unique_ptr<Object>`: represents the `Object` detected (what was detected)
* `Vector2`: represents the position of the `Object` (where it was detected)

The position of `Detection` uses global `World` coordinates. 

## References

* [Sensor](Sensor.md)
* [Object](../../Object.md)
* [World](../../World.md)