# Differetial Drive

This is the differential-drive movement device. It moves ```Robot``` around by converting two ```Motor```s angular
velocities into linear/angular velocities. These linear/angular velocities applies to ```Robot``` and adjust its
transform accordingly. 

The ```Motor```s it references must be attached **before** attaching this device. If it is not, it will fail to 
attach and will throw an exception. This device does not own the ```Motor```s it references. Rather it stores
pointers to the ```Motor```s the robot owns, based off the specified id. 

The update priority of this device is 50. This is so ```Motor```s are updated before is, allowing it to apply to
the current velocities and not old velocities. 

## References

- [Device](../Device.md)