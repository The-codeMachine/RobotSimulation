# Object

The ```Object``` class represents something in the ```World```. It holds
positional, rotational, and graphical information about every object
inside the world. 

You can construct an object from nothing, or a specified ```Transform```, and
```name```. All Objects and their subclasses require a default constructor
and a deserialization function. After initial construction, the object will be 
deserialized. Deserialization paramters are passed as JSON objects. 

## File Saving, Loading & Serialization

File saving and loading is handled through the ```World``` class. The ```World``` class will 
construct all Objects necessary, with help from the ```Factory``` class. 

However, serialization and deserialization must be handled through this class, and its respected
subclasses. Because we use JSON files to save the world, serialization must return JSON objects.
We use ```nlohmann::json``` for our parsing. Deserialization expects JSON objects as well. 

An example of a Robot represented in JSON might look like the following:

```json
{
    "type": "Robot",
    "transform": {
        "x": 5,
        "y": 2,
        "rotation": 0
    },
    "data": {
        "devices": [
            {
                "type": "Motor",
                "id": [0],
                "power": 0.75
            },
            {
                "type": "Camera",
                "id": [1],
                "fov": 90
            }
        ]
    }
}
```

## Object Access Functions 

You can access an object's specific transform, type, and world reference through
the functions:

 - ```transform```
 - ```world```
 - ```name```

respectfully. The ```transform```, ```world```, and ```name``` functions can return a constant 
or non-constant reference. 

# Vector2

```Vector2``` represents one 2D position in the world. It consists of two
```uint32_t```; x and y, to encapsulate its position. 

# Transform

A ```Transform``` encapsulates an Object's rotational, and positional value. 
You can construct a Transform from nothing, or a specified rotational and
Vector2. Its rotational value is represented as a ```double``` where 0 is
North and 45 is NW. 

## References

- [World](World.md)
- [Factory](Factory.md)