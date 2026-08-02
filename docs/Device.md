# Device

This is the base class to all devices. It provides common functionality like types and
ids. A device is meant to connect to a Robot providing expanded functionality. 

Both ```type``` and ```id``` are unique only to that type of Device. You can have to two
different classes with the same ```id```, but each must have its own unique ```type``` 
identifier. 

Device has a pointer reference to its ```Robot``` owner. The ```Robot``` class is a 
friend class. 

```serialization``` is called to serialize the object to JSON. We use ```nlohmann::json``` for 
our JSON parsing. 

```deserialization``` is called when constructing an object from JSON. You can expect the JSON
to match the serialization. ```World``` will capture those errors and print them to inform 
you that the JSON file is corrupt. 

```onAttach``` is called when attaching this device to a ```Robot``` (when ```addDevice``` is
called).

```onDetach``` is called when removing a device from a ```Robot``` (e.g. during destruction).
Currently, ```onDetach``` is not called anywhere inside ```Robot``` or ```Device```. (including 
in the destructor). 

```update``` is where most of the functionality occurs, based off the ```deltaTime``` it should
update something about the device, or robot. 

### References

- [Robot](Robot.md)