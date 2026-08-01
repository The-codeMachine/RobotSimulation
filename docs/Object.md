# Object

The ```Object``` class represents something in the ```World```. It holds
positional, rotational, and graphical information about every object
inside the world. 

You can construct an object from nothing, or a specified ```Transform```, and
```ObjectType```. All Objects and their subclasses require a default constructor
and a deserialization function. After initial construction, the object will be 
deserialized. Deserialization paramters are passed as ```std::unordered_map<uint32_t, std::string>```. 

Serialization can also be done. We expect this to be one continuous string. It will
follow this format:
 - all arguments are separated by ```,```
 - arguments where they take arguments, e.g. Devices arguments are specified by ```[```; you can escape using ```]```. 

You can access an object's specific transform, type, and world reference through
the functions:

 - ```transform```
 - ```world```
 - ```getType```

respectfully. The ```transform``` and ```world``` functions can return a constant 
or non-constant reference. This differes from ```getType``` which returns a copy
of the object's type.

You can also access what this Object would look like as a character (for graphic 
representation). This is through the ```toChar``` function. 

## ObjectType

```ObjectType``` represents a type of object. It encapsulates a ```uint8_t```. 
Current list of all types with their graphical representation is below:

 - EMPTY: (' ')
 - WALL: ('#')
 - ROBOT ('R')

# Vector2

```Vector2``` represents one 2D position in the world. It consists of two
```uint32_t```; x and y, to encapsulate its position. 

# Transform

A ```Transform``` encapsulates an Object's rotational, and positional value. 
You can construct a Transform from nothing, or a specified rotational and
Vector2. Its rotational value is represented as a ```double``` where 0 is
North and 45 is NW. 

## References

- [World](World.md)
