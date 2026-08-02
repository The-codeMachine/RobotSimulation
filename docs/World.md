# World

The world class encapsulates all objects within a world. This includes:

 - The Robot
 - Walls
 - Interactable objects
 - Custom objects

All objects are accessible based off their Transform (specifically the ```Vector2``` location).
The world owns the objects. The world is copyable and moveable. 

## Construction

The world can be constructed from a JSON file or object. We use ```nlohmann::json``` for our JSON
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
            "transform": {
                "x": 0,
                "y": 0,
                "rotation": 0
            }
        },
        {
            "type": "Wall",
            "glyph": "#",
            "transform": {
                "x": 1,
                "y": 0,
                "rotation": 0
            }
        },
        {
            "type": "Wall",
            "glyph": "#",
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

Internally, the Objects are stored as a vector of unique pointers; ```std::vector<std::unique_ptr<Object>>```.

You can update a location using the ```update``` function. This allows you to change which object
is at that location. This function only takes an ```std::unique_ptr<Object>``` as a parameter because Objects own where
they are located, and that can simply be accessed. 

## To String

The string conversion function converts the current world's situation into a string. It will not be as detailed as the
JSON output, and the glyphs used to print are not unique and may be reused since they are only characters. We expect
to be making a true renderer on top of this library later that will base its rendering off JSON output, and we will 
include things like updates packets in JSON. Currently, however the ```toString``` function is used exclusively for
debugging and simple conversions. 

## References

- [Object](Object.md)
- [Robot](Robot.md)
