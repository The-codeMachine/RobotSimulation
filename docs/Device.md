# Device

This is a very basic class. It is the super class to all Device subclasses. 
It provides ID handling, and ```onAttach```, ```onDetach```, and ```update``` 
functions. 

It has a pointer reference to its ```Robot``` owner. The ```Robot``` class is a 
friend class. 

```onAttach``` is called when attaching this device to a ```Robot``` (when ```addDevice``` is
called).

```onDetach``` is called when removing a device from a ```Robot``` (e.g. during destruction).
Currently, ```onDetach``` is not called anywhere inside ```Robot``` or ```Device```. (including 
in the destructor). 

```update``` is where most of the functionality occurs, based off the ```deltaTime``` it should
update something about the device, or robot. 

## References

[Robot](Robot.md)