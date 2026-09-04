# ChangeSink & ChangeEvent

This document defines what a ChangeSink and ChangeEvent are. 

## ChangeSink

A `ChangeSink` simply does something with a `ChangeEvent`. Each subclass defines what exactly
it does with the `ChangeEvent`. This can really be anything from printing it out to the console
for debugging to releasing it to a port for other programs to listen to. 

`ChangeSink` has a `publish` function. `publish` will release the `ChangeEvent` passed to it
as a parameter. 

## ChangeEvent

`ChangeEvent` is a structure type. It represents a new change that has occurred in the simulation.
This can be anything from an object moving, to a robot's motor adjusting its throttle. These changes
are defined by the `type` variable. A simple string that represents the change. 

`ChangeEvent` uses `sequence` to define when in the simulation this happened. Whether it was before or
after a different event occurred, even if the other event has not arrived yet. This allows other programs
to stay on track with the simulation and not jump ahead, or fall behind. 

`ChangeEvent`'s data is passed as a JSON type. This simply represents the changed data. Very little 
information should be passed. For example, when an object moves it should only consist of the new Transform,
and the specific object that moved (likely by its unique id).

### References

- [Object](Object.md)
- [World](World.md)