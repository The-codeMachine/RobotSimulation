# Ray

`Ray.hpp` defines the basic types used to represent a ray and the result of a raycast.

## Ray

A `Ray` represents a point and a direction:

* `origin` — The starting position of the ray.
* `direction` — The direction in which the ray travels.

The direction should normally be normalized.

A ray has no inherent maximum length. A maximum sensing distance can instead be supplied to the operation performing the raycast.

## RaycastResult

`RaycastResult` describes the first object intersected by a ray.

It contains:

* `object` — The object that was hit. This is a non-owning pointer.
* `position` — The position at which the ray intersected the object.
* `normal` — The surface normal at the intersection point.
* `distance` — The distance from the ray's origin to the intersection.

`RaycastResult` represents a measurement result; it does not own the detected object.

## Purpose

These types are intentionally independent of sensors. A raycast is a general world query and can be used by multiple systems, including distance sensors, lidar, and other future sensors.
