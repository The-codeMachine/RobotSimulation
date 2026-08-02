# Object

The ```Object``` class represents something in the ```World```. It holds
positional, rotational, and graphical information about every object
inside the world. 

You can construct an object from nothing, or a specified ```Transform```,
```name```, and ```glyph```. All Objects and their subclasses require a serialization
and a deserialization function. After initial construction, the object will be 
deserialized. Deserialization paramters are passed as JSON objects. 

The ```glyph``` is used to print the World with prettier formatting. The ```name``` is
used for construction specification and must be unique to all subclasses, while a 
glyph can be reused as many times as possible, it is simply for pretty printing. 

## File Saving, Loading & Serialization

File saving and loading is handled through the ```World``` class. The ```World``` class will 
construct all Objects necessary, with help from the ```Factory``` class. 

However, serialization and deserialization must be handled through this class, and its respected
subclasses. Because we use JSON files to save the world, serialization must return JSON objects.
We use ```nlohmann::json``` for our parsing. Deserialization expects JSON objects as well. 

An example of a Wall represented in JSON might look like the following:

```json
{
    {
        "type": "Wall",
        "glyph": "#",
        "transform": {
            "x": 0,
            "y": 5,
            "rotation": 0
        }
    },
}
```

## Object Access Functions 

You can access an object's specific transform, world reference, name, and glyph respectfully 
through the following:

 - ```transform```
 - ```world```
 - ```name```
 - ```glyph```

The ```transform```, ```world```, ```name```, and ```glyph``` functions can return a constant 
or non-constant reference. 

# Vector2

```Vector2``` represents one 2D position in the world. This structure encapsulates the 2D position 
of one ```Object```. ```Vector2``` has:

 - x (```uint32_t```)
 - y (```uint32_t```)

# Transform

A ```Transform``` encapsulates an Object's rotational, and positional value. 
You can construct a Transform from nothing, or a specified rotational and
Vector2. Its rotational value is represented as a ```double``` where 0 is
North and 45 is NW. Following conventional degrees. 

### References

- [World](World.md)
- [Factory](Factory.md)