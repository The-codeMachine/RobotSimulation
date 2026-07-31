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

When constructing from a file, the world expects a file similar to this:

```
0,0,0,1;1,0,0,1;2,0,0,1;3,0,0,1;4,0,0,1;
0,0,0,1;1,1,0,0;2,1,0,0;3,1,0,0;4,1,0,1;
0,0,0,1;1,2,0,0;2,2,0,0;3,2,0,0;4,2,0,1;
0,0,0,1;1,3,0,0;2,3,0,0;3,3,0,0;4,3,0,1;
0,0,0,1;1,4,0,0;2,4,0,0;3,4,0,0;4,4,0,1;
0,0,0,1;1,5,0,0;2,5,0,0;3,5,0,0;4,5,0,1;
0,0,0,1;1,6,0,0;2,6,0,0;3,6,0,0;4,6,0,1;
0,0,0,1;1,7,0,0;2,7,0,0;3,7,0,0;4,7,0,1;
0,0,0,1;1,8,0,1;2,8,0,1;3,8,0,1;4,8,0,1;
```

Would construct something like this:

```
#####
#   #
#   #
#   #
#   #
#   #
#   #
#   #
#####
```


## Object-Accessing

You can access a copy of the ```Object``` using the ```at``` function. This takes a ```Vector2``` position
as a parameter. 

Internally, the Objects are stored as a vector of unique pointers; ```std::vector<std::unique_ptr<Object>>```.

You can update a location using the ```update``` function. This allows you to change which object
is at that location. This function only takes an ```std::unique_ptr<Object>``` as a parameter because Objects own where
they are located, and that can simply be accessed. 

## To String

The string conversion function converts the current world's situation into a string. This string can be loaded
into a file to save it. It will be able to construct all objects from the string. However, if the string includes
things like ```R``` for robot, it will not construct a robot object. 

## References

- [Object](Object.md)
- [Robot](Robot.md)

### TODO

 - During string construction, make it so that based off the character the specific Object will be constructed, e.g. ```R``` would construct a robot 
