# World

The `World` class encapsulates all objects within a world, including:

- The `Robot`
- Walls
- Interactable objects
- Custom objects

Objects are stored by their `Vector2` world position in a `std::unordered_map`. The world owns the objects. It is
non-copyable and non-moveable.

## Construction

The world can be constructed from a JSON file or JSON object. It uses `nlohmann::json` for parsing. World files
follow a format similar to:

```json
{
    "version": 1,
    "ROW_SIZE": 20,
    "ROW_AMOUNT": 10,
    "objects": [
        {
            "type": "Wall",
            "glyph": "#",
            "unique_id": "Wall-1",
            "transform": {
                "x": 0,
                "y": 0,
                "rotation": 0
            }
        },
        {
            "type": "Wall",
            "glyph": "#",
            "unique_id": "Wall-2",
            "transform": {
                "x": 1,
                "y": 0,
                "rotation": 0
            }
        },
        {
            "type": "Wall",
            "glyph": "#",
            "unique_id": "Wall-3",
            "transform": {
                "x": 2,
                "y": 0,
                "rotation": 0
            }
        }
    ]
}
```

## Object Access

Objects can be accessed by their `Vector2` position using the world's access functions. Internally, the objects are
stored as:

```cpp
std::unordered_map<Vector2, std::unique_ptr<Object>>
```

The world owns each object through its `std::unique_ptr`.

A location can be updated using `update()`. This function accepts an `std::unique_ptr<Object>` because an
object's transform determines its world location.

## Sensing

`World` can sense objects using a shape. It traverses the shape's world-space coverage and returns a
`std::vector<Detection>`. These detections can be used to construct an `Image` or other sensor-related data.

## Collision Casting

`World::cast()` performs broad-phase candidate selection and delegates the actual geometric collision test to
the candidate object's collider.

The collision query follows two phases:

1. **Broad phase:** Calculate the trajectory's swept AABB and use the world's `map_` to find objects in the
   grid cells covered by that AABB.
2. **Narrow phase:** Call the candidate collider's `cast()` implementation with the trajectory. The collider
   performs the geometric test and may return a `CollisionResult`.

The intended flow is:

```text
Trajectory
    |
    v
Calculate swept AABB
    |
    v
World::cast()
    |
    v
Find candidate objects using map_
    |
    v
Collider::cast()
    |
    v
CollisionResult
(earliest collision)
```

### Broad phase

The broad phase obtains the trajectory bounds:

```cpp
const AABB bounds = trajectory.bounds();
```

It converts the minimum and maximum coordinates to grid-cell coordinates using `std::floor`, then looks up each
covered coordinate in `map_`. This avoids iterating over every object in the world for every collision cast.

Objects that are null, equal to the ignored object, or empty are skipped. Each remaining candidate is passed to
its collider for narrow-phase testing.

If multiple candidates report collisions, `World::cast()` retains the result with the smallest normalized
collision time.

### Grid and bounds assumptions

The current map-based lookup is correct only under specific spatial assumptions:

- World positions correspond to integer grid coordinates.
- Each collider is contained within the grid cell associated with its object.
- The trajectory bounds conservatively cover the moving object's collision geometry.
- The trajectory bounds, collider geometry, and map coordinates use the same world coordinate system.

The current `Trajectory::bounds()` implementations describe the trajectory of the reference point. They do not,
by themselves, include the moving object's collider dimensions. A reference-point AABB is therefore not
automatically a swept AABB for a robot with non-zero size.

If colliders can extend beyond their object's grid cell, looking up only the cells inside the trajectory AABB can
miss a collision. In that case, the broad phase must be expanded conservatively or replaced with a spatial index
that registers colliders in every cell they overlap.

### Scope of the collision query

The broad phase only selects possible collision candidates. It does not establish that a collision occurred.
That decision belongs to the collider's narrow-phase `cast()` implementation.

Likewise, returning a collision does not automatically resolve it. Collision response remains the responsibility
of the movement or physics system.

## Visual Representation

The `World` has infinite bounds, so it cannot be represented in its entirety using text. Worlds are saved to and
loaded from JSON files. Third-party programs may visualize them, for example through HTML or Godot.

`ChangeSink`s can forward real-time world updates to a specific port or through other mechanisms.

## References

- [Object](Object.md)
- [Robot](Robot.md)
- [Collisions](Collisions.md)
- [Sensor](Devices/Sensors/Sensor.md)
- [SensorShape](Devices/Sensors/SensorShape.md)
- [ChangeSink & ChangeEvent](ChangeSink&ChangeEvent.md)
