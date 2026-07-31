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

## References

- [Object](Object.md)
- [Device](Device.md)

### TODO

 - Make a file saving system for the robot, where it saves the robot's information to a file
 - Once a file saving system is made for the robot make a construction system for the robot based off a file