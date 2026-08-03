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

Id is represented as a string. 

Device creational and retrival might be dangerous. They explore invalid states differently: 

- Device creation will always throw exceptions if anything fails (including two ```Device```s with the same name).
- Device retrival will return ```nullptr``` if it fails. 

You can create a ```Device``` through the following functions:

  - ```addDevice(const std::string& id, Args&&... args)```: this will create and insert the Device into
   the Robot and return a reference (if possible)
  - ```addDevice(std::unique_ptr<Device> device)```: this will insert the Device into the Robot and return 
    a reference (if possible)


Devices are sorted based off their update priority everytime a new device is added. This ensures that the devices are
executed squentially according to their own input. This is not a static function but uses a static way. We are still
determining whether or not to allow the users to specify exactly how long they want that device. We could simply have
a default and then the user can set it on their own. 

## File Loading

File loading and saving is done through JSON files. JSON is parsed through ```nlohmann::json```. An
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

### References

- [Object](Object.md)
- [Device](Device.md)
