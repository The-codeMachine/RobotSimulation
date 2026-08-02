# Robot

The robot class inherits from ```Object```. The robot is a simple object that
encapsulates many devices. These devices give the robot its abilities. The robot
supports functionality such as movement and view, but the implementation is
derived from specific ```Device```s.

## Devices

The robot allows you to access the devices through a reference. You can add a device through
a preconstructed unique pointer, or through an id with the arguments to the device's constructor.
Adding a device will return a reference to that device. 

To access, or create a device you must specific what type of Device it is, as well as its id. 
The id is unique only to that type of device. E.g. you can have one ```Motor``` named "Left Motor", 
but can also have a ```SpecialMotor``` called "Left Motor." 

The id is a ```std::vector<unsigned char>```, this is so you can use numbers as well as strings. 

If getting/creating a device was unsuccessful, it will throw an exception. 

## File Loading

File loading and saving is done through JSON files. Json is parsed through ```nlohmann::json```. An
example of a robot would look like the following:

```json
{
    "type": "Robot",
    "glyph": "R",
    "transform": {
        "x": 10,
        "y": 5,
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

All file parsing is done by the ```World``` class. Specific construction is done by the ```Factory``` class.
Each object is responsible for serialization and deserialization. As such, ```Robot``` also has these functions,
as well as all devices.

## References

- [Object](Object.md)
- [Device](Device.md)
