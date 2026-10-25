# Vector2

`Vector2` represents a two-dimensional position or vector. `Vector2` also has a hashing 
functionality allowing to serve as a key for `unordered_map`s. 

It provides equality comparison and is used throughout the collision system for:

* Positions
* Velocities
* Accelerations
* Directions
* Collision normals

The collision implementation also provides vector operations including:

* Addition
* Subtraction
* Scalar multiplication
* Dot product
* Length
* Normalization
* Component-wise clamping