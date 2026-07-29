# World

The world class encapsulates all objects within a world. This includes:

 - The Robot
 - Walls
 - Interactable objects
 - Custom objects

All objects are accessible based off their Transform (specifically the ```Vector2``` location).
The world owns the objects. The world is copyable and moveable. 

## Construction

You can construct this world from a string, or a file. The constructor will throw if
there are any parsing issues. Example of valid strings include:

```
#########
#       #
#       #
#       #
#########
```

```
#########
#       #
#   R   #
#       #
#########
```

When constructing from a file, the world expects a file similar to this. 

## Object-Accessing

You can access a copy of the ```Object``` using the ```at``` function. This takes a ```Vector2``` position
as a parameter. 

You can update a location using the ```update``` function. This allows you to change which object
is at that location. This function only takes an ```Object``` as a parameter because Objects own where
they are located, and that can simply be accessed. 

## To String

The string conversion function converts the current world's situation into a string. This string can be loaded
into a file to save it. It will be able to construct all objects from the string. However, if the string includes
things like ```R``` for robot, it will not construct a robot object. 

## References

[Object](Object.md)

### TODO

 - Add a way to represent the orientation of an object
 - During string construction, make it so that based off the character the specific Object will be constructed, e.g. ```R``` would construct a robot 
 - Make ```at``` return a reference, and make a const reference function as well
 - Find a way to represent an Object's rotational value inside the World's output. Not sure if it is a world issue, or object issue
