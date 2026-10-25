# World

The world class encapsulates all objects within a world. This includes:

 - The Robot
 - Walls
 - Interactable objects
 - Custom objects

All objects are accessible based off their `Vector2` position (these are stored in a `std::unordered_map`).
The world owns the objects. The world is non-copyable and non-moveable. 

## Construction

The world can be constructed from a JSON file or object. We use `nlohmann::json` for our JSON
parsing. The world files will follow formats similar to that of the following:

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

## Object-Accessing

You can access a copy of the ```Object``` using the ```at``` function. This takes a ```Vector2``` position
as a parameter. 

Internally, the Objects are stored as a map of unique pointers; `std::unordered_map<Vector2, std::unique_ptr<Object>>`.

You can update a location using the `update` function. This allows you to change which object
is at that location. This function only takes an `std::unique_ptr<Object>` as a parameter because Objects own where
they are located, and that can simply be accessed. 

## Sensing

`World` can sense things based off a shape. `World` traverses the shape's world location within the `World`. Based off
these result calculations it will return `std::vector<Detection>`. This can be used to construct an `Image`, or other
sensor related objects. 

## Visual Representation

Because the `World` has infinite bounds it is impossible to represent with text. `World` is therefore only accessible
through files. `World`s can be saved and loaded from these files. These are JSON. Third-party programs may represent 
these (e.g. through HTML, Godot, etc.). This is also the purpose of `ChangeSink`s. These forward real-time updates
to a specific port (or other methods).

### References

- [Object](Object.md)
- [Robot](Robot.md)
- [Sensor](Devices/Sensors/Sensor.md)
- [SensorShape](Devices/Sensors/SensorShape.md)
- [ChangeSink & ChangeEvent](ChangeSink&ChangeEvent.md)
