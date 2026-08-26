# Sensor Shape

`SensorShape.hpp` defines the geometric regions used by sensors to determine which parts of the simulated world are within their sensing area.

A `SensorShape` represents the **geometry of a sensor's sensing region**. It does not perform a world query itself and does not determine what an object contains or represents.

## SensorShape

`SensorShape` is the abstract base class for all sensor shapes.

Every sensor shape contains an `origin` represented by a `Transform`. The origin provides a position and orientation for shapes whose geometry is defined relative to an origin.

Concrete shapes implement:

```cpp
virtual bool contains(Transform point) const noexcept = 0;
```

`contains()` determines whether a specified point lies within the shape.

A return value of `true` means that the point is inside the shape's boundaries; `false` means that it is outside.

## Coordinate System

`SensorShape` operates on positions represented by `Transform`.

The coordinates supplied to `contains()` are interpreted according to the coordinate system expected by the particular shape implementation.

Shapes that are defined relative to their origin, such as `SensorShapeCone` and `SensorShapeBall`, use the shape's origin when determining containment.

`SensorShapeQuadratic` instead stores its four vertices directly and therefore does not derive its geometry from the origin.

The distinction is important: the base class provides an origin, but **not every concrete shape necessarily uses the origin to define its boundaries**.

## Containment

`contains()` is a point-containment operation.

It does not:

* Search the `World`.
* Find objects inside the shape.
* Return the objects detected by a sensor.
* Enumerate every position inside the shape.
* Perform collision detection between arbitrary objects.

A world-level sensing operation can use `contains()` as part of determining whether objects or positions fall within a sensor's sensing region.

For example, a world query may obtain the relevant objects and test their positions against a sensor shape.

## SensorShapeQuadratic

`SensorShapeQuadratic` represents a four-vertex polygon.

The four vertices are stored in an array:

```cpp
std::array<Transform, 4>
```

The vertices are ordered counter-clockwise when assigned to the shape. This allows the containment calculation to operate on a consistent polygon representation even when the vertices supplied to the constructor are not already ordered.

The shape is defined directly by its vertices rather than by its origin.

Despite the class name, this shape is not a quadratic curve or quadratic surface. It is a polygon with four vertices. The name should therefore be understood as project-specific terminology rather than a mathematical description.

If this class is intended to represent a general four-sided polygon, a name such as `SensorShapeQuadrilateral` would be more precise.

### Vertices

The vertices can be accessed through:

```cpp
const std::array<Transform, 4>& vertices() const noexcept;
```

They can be replaced using `setVertices()`.

When new vertices are assigned, they are ordered consistently before being stored.

### Containment

`contains()` determines whether a point lies within the four-sided polygon represented by the stored vertices.

The polygon is assumed to be a valid non-self-intersecting quadrilateral.

## SensorShapeCone

`SensorShapeCone` represents a directional sensing region originating from `origin_`.

The cone is defined by:

* `fov` — the field of view, expressed as an angle.
* `range` — the maximum sensing distance from the origin.
* `origin` — the position and orientation from which the cone extends.

Unlike `SensorShapeQuadratic`, the geometry is derived from the origin's position and rotation.

The origin's orientation determines the direction in which the cone points.

### Field of View

The field of view determines the angular width of the sensing region.

A point must lie within the cone's angular boundaries to be contained by the shape.

### Range

The range determines the maximum distance from the origin at which a point can be contained.

A point beyond the configured range is therefore outside the sensing region regardless of its direction.

### Containment

`contains()` evaluates both the direction and distance of the supplied point relative to the cone's origin.

Conceptually, a point is contained when:

1. It is within the cone's maximum range.
2. Its direction from the origin falls within the configured field of view.

## SensorShapeBall

`SensorShapeBall` represents a circular sensing region centered on its origin.

Although the class is named `Ball`, the current two-dimensional sensor geometry is a **circle**, not a three-dimensional sphere.

The shape is defined by:

* `origin` — the center of the circle.
* `radius` — the maximum distance from the center.

A point is contained when its distance from the origin is less than or equal to the configured radius.

If the simulation remains strictly two-dimensional, `SensorShapeCircle` would be a more mathematically precise class name than `SensorShapeBall`.

### Radius

The radius determines the size of the sensing region.

Increasing the radius increases the area in which points can be contained.

### Containment

`contains()` checks the distance between the supplied point and the shape's origin.

Conceptually:

```text
distance(point, origin) <= radius
```

means that the point is contained by the shape.

## Shape Registration

Concrete sensor shapes provide registration functions:

```cpp
static void registerSensorShapeQuadratic();
static void registerSensorShapeCone();
static void registerSensorShapeBall();
```

These functions register the corresponding concrete type with the `SensorShape` factory.

This allows a sensor shape to be constructed dynamically from serialized type information without requiring the caller to explicitly know the concrete shape class.

## Serialization

`SensorShape` and its concrete subclasses support serialization and deserialization through `nlohmann::json`.

The base serialization contains common shape information such as the origin and type.

Concrete shapes extend this representation with their specific geometry.

For example:

* `SensorShapeQuadratic` serializes its four vertices.
* `SensorShapeCone` serializes its field of view and range.
* `SensorShapeBall` serializes its radius.

Deserialization reconstructs the corresponding shape state from the serialized representation.

## Relationship With Sensors

A `Sensor` owns a `SensorShape`.

The responsibilities are deliberately separated:

**Sensor**

* Determines what type of measurement is produced.
* Stores the sensor's local transform.
* Performs sensor-specific sensing.
* Stores the resulting measurement.

**SensorShape**

* Defines the geometric sensing region.
* Determines whether a position lies within that region.
* Stores shape-specific geometric configuration.

**World**

* Owns the simulated environment.
* Provides access to objects and their world positions.
* Performs world-level queries needed to determine what exists within a sensing region.

This separation allows the same shape to potentially be reused by multiple sensor types without coupling the geometry to a particular measurement.

## Current Shape Types

The currently implemented sensor shapes are:

| Shape                  | Geometry            | Origin-dependent |
| ---------------------- | ------------------- | ---------------- |
| `SensorShapeQuadratic` | Four-vertex polygon | No               |
| `SensorShapeCone`      | Directional cone    | Yes              |
| `SensorShapeBall`      | Circle              | Yes              |

## Naming Considerations

Two current names are potentially misleading:

* `SensorShapeQuadratic` sounds like a quadratic mathematical shape, but the implementation represents a four-vertex polygon.
* `SensorShapeBall` sounds three-dimensional, but the implementation represents a two-dimensional circle.

If these classes are intended to remain two-dimensional, `SensorShapeQuadrilateral` and `SensorShapeCircle` would communicate their purpose substantially better.

Renaming them now would also be considerably cheaper than changing them after they become part of serialized world files, factory registrations, tests, and other APIs.

## References

* [Sensor](Sensor.md)
* [World](../../World.md)
