# Sensor Shape 

Sensor shape is a specific geometric shape specifying how a sensor scans an area in `World`. 
 
`SensorShape` simply encapsulates the positions of the sensor's "view." It defines the boundaries of a 
`Sensor`'s view. 

All coordinates handled by `SensorShape` are global.

## Contain

`SensorShape`'s `contain` function checks whether a specific `Vector2` position is within the defined 
boundaries of this particular `SensorShape`. 

`contain` takes global coordinates. `contain` returns true if the inputted `Vector2` position is within
the `SensorShape` otherwise it will return false. 

`SensorShape` allows access to all positions it contains (from whole numbers)

## Basic Sensor Shapes

Currently, the following sensor shapes have been made:

* `Rectangle`: defined by a width and length
* `Cone`: defined by a range and FOV (Field of View)
* `Circle`: defined by a radius

## References

* [Sensor](Sensor.md)