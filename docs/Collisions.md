# Collision System

The collision system provides the geometric and mathematical primitives required to determine whether a moving
`Object` intersects a `Collider` during a physics timestep.

The system is designed around three primary concepts:

* **Trajectories** describe where an object moves over time.
* **Colliders** describe the geometry an object can collide with.
* **Collision casting** determines whether a trajectory intersects a collider and, if so, when the first
  collision occurs.

The collision system does not own the objects or colliders involved in a collision. It performs collision
queries and returns a `CollisionResult` describing the detected collision.

## Design

The collision system is intentionally separated from the objects that use it.

A moving object does not need to know how a particular collider is represented. Instead, it provides a
`Trajectory` to the collision system, which evaluates that trajectory against a `Collider`.

Conceptually, the Object's movement is translated into a Trajectory. This Trajectory is casted. This cast
uses the Collider system for its physics calculations.

This allows movement devices such as `DifferentialDrive` to use collision detection without implementing
collision mathematics themselves.

## Vector2

`Vector2` represents a two-dimensional position or vector.

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

## Trajectories

A `Trajectory` represents the movement of an object over a single physics timestep.

The trajectory interface allows the collision system to query the object's position at any normalized time
between the beginning and end of the timestep.

The time parameter `t` is normalized:

```text
t = 0
```

represents the beginning of the timestep, while:

```text
t = 1
```

represents the end.

This allows collision detection to remain independent of the actual timestep duration.

The trajectory converts normalized time into real elapsed time:

```text
time = t × deltaTime
```

Different movement systems can implement different trajectory types while remaining compatible with the same
collision system.

For example:

* `AcceleratedTrajectory` represents constant acceleration.
* `DifferentialDriveTrajectory` represents differential-drive movement along a circular or straight path.

## Accelerated Trajectory

`AcceleratedTrajectory` represents motion with constant acceleration.

It stores:

* Initial position
* Initial velocity
* Constant acceleration
* Timestep duration

The position at time `t` is calculated using the standard kinematic equation:

```text
p(t) = p₀ + v₀t + ½at²
```

Applied independently to the two dimensions:

```text
x(t) = x₀ + vₓt + ½aₓt²

y(t) = y₀ + vᵧt + ½aᵧt²
```

The instantaneous velocity is:

```text
v(t) = v₀ + at
```

This trajectory is useful for objects whose movement can be approximated using constant linear acceleration
during a timestep.

## Colliders

A `Collider` represents the geometry of an object that can participate in collision detection.

The collider interface provides geometric queries such as:

* Determining whether a point is inside the collider.
* Determining the signed distance from a point to the collider.

The collision system does not need to know the concrete collider type. It interacts with the abstract
`Collider` interface instead. `Collider`s can be made using a Collider factory. Access through 
`Collider::Collider_Factory` works just like `Object` and `Device` factories.  

This makes it possible to add additional collider types without modifying the trajectory casting algorithm.

Examples of collider implementations include:

* `AABBCollider`
* `CircleCollider`

## Signed Distance

The primary geometric operation used by trajectory casting is `signedDistance()`.

A signed distance function returns:

* A **positive** value when the point is outside the collider.
* `0` when the point is on the collider's boundary.
* A **negative** value when the point is inside the collider.

Therefore:

```text
distance > 0  → outside
distance = 0  → boundary
distance < 0  → inside
```

This provides a generic way for the collision system to determine whether a trajectory has entered a collider
without knowing the collider's specific geometry.

## AABB Collider

`AABBCollider` represents an axis-aligned bounding box.

It is defined by two points:

```text
minimum = (minX, minY)
maximum = (maxX, maxY)
```

The constructor requires:

```text
minX <= maxX
minY <= maxY
```

Otherwise, `std::invalid_argument` is thrown.

### Point Containment

A point is contained by the AABB when:

```text
minX <= x <= maxX
minY <= y <= maxY
```

The boundaries are therefore considered part of the collider.

### Signed Distance

For a point outside the AABB, the signed distance is the Euclidean distance to the closest point on the box.

For a point inside the AABB, the signed distance is the negative distance to the closest edge.

For example, a point located exactly on the boundary has a signed distance of:

```text
0
```

This makes the AABB compatible with the generic trajectory casting algorithm.

## Circle Collider

`CircleCollider` represents a circle defined by:

* Center position
* Radius

The radius cannot be negative.

A zero-radius circle is valid and effectively represents a single point.

### Point Containment

A point is contained by the circle when its distance from the center is less than or equal to the radius:

```text
|p - center| <= radius
```

### Signed Distance

The signed distance is:

```text
distance(p) = |p - center| - radius
```

Therefore:

```text
distance > 0  → outside
distance = 0  → on the circle
distance < 0  → inside
```

## Trajectory Casting

`castTrajectory()` is responsible for determining whether a trajectory intersects a collider.

It takes:

* A `Trajectory`
* A `Collider`
* An optional `Object*` associated with the collider
* A collision epsilon

The function returns:

```text
std::optional<CollisionResult>
```

If no collision occurs, it returns `std::nullopt`.

If a collision occurs, it returns the earliest detected collision.

### Continuous Trajectory Evaluation

The trajectory is not treated as a single movement from its starting point to its ending point.

Instead, the collision system samples the trajectory at multiple points throughout the timestep.

The current implementation uses:

```text
SAMPLE_COUNT = 128
```

samples.

The normalized timestep is divided into 128 intervals:

```text
0
1/128
2/128
...
127/128
1
```

At every sample, the signed distance between the trajectory position and the collider is calculated.

This allows curved trajectories, such as differential-drive trajectories, to be checked without requiring the
collision system to understand their mathematical form.

## Collision Detection Algorithm

The algorithm first evaluates the trajectory at:

```text
t = 0
```

If the object is already inside or exactly on the collider, a collision at `t = 0` is immediately returned.

Otherwise, the system proceeds through the trajectory samples.

For each pair of consecutive samples:

```text
previousT → currentT
```

the signed distances are examined.

If the trajectory changes from:

```text
positive → zero/negative
```

then the trajectory has entered the collider during that interval.

The collision therefore lies somewhere inside:

```text
[previousT, currentT]
```

The system then performs root finding over that interval to determine a more accurate collision time.

## Root Finding

`findRoot()` uses the **bisection method** to find a point where a function reaches zero.

The collision system supplies the function:

```text
f(t) = collider.signedDistance(trajectory.position(t))
```

A collision boundary occurs when:

```text
f(t) = 0
```

Because the system has already detected a transition from outside to inside, the interval is expected to contain
a root.

Bisection repeatedly divides the interval in half until either:

* The function is sufficiently close to zero.
* The interval is smaller than the requested epsilon.

The method is deliberately simple and robust. It does not require derivatives of the trajectory or collider
and therefore works with arbitrary implementations of the `Trajectory` and `Collider` interfaces.

## Collision Result

When a collision is found, the system calculates:

* Normalized collision time
* Collision position
* Collision normal
* Colliding object

The collision time is normalized to `[0, 1]`, matching the trajectory interface.

The collision position is obtained directly from the trajectory:

```text
collisionPosition = trajectory.position(collisionTime)
```

This ensures that the returned position corresponds exactly to the reported point in the trajectory.

## Collision Normal

The collision normal is currently an approximation.

The generic `Collider` interface currently provides a signed distance but does not provide a method such as:

```text
normalAt(position)
```

A signed distance alone does not reliably provide the surface normal for every possible collider.

Therefore, the current implementation approximates the normal using the direction of movement around the collision:

```text
before = trajectory.position(collisionTime - Δt)
after  = trajectory.position(collisionTime + Δt)

normal = normalize(after - before)
```

This produces the direction of travel through the collision point.

**This is not a geometrically correct surface normal.**

It is only a temporary fallback and should not be used for physically accurate collision response.

The intended future design is for `Collider` to expose a surface-normal query, allowing the collision system to
return the actual geometric normal at the collision point.

## Starting Inside a Collider

If the trajectory begins inside or exactly on the boundary of a collider, the collision system immediately
returns a collision at:

```text
t = 0
```

No attempt is made to determine a normal in this case.

This is intentional. The object may have started inside a collider because of initial placement, another collision
response, or an externally modified transform. Inventing a normal in this situation could result in incorrect
collision responses.

## Collision Precision

The collision system uses two separate concepts of precision.

### Sampling precision

The trajectory is initially sampled 128 times.

This determines how small an interval the system must search before performing root finding.

A larger sample count makes the system less likely to miss a collision but increases the number of
`trajectory.position()` and `collider.signedDistance()` evaluations.

### Root-finding precision

Once a collision interval has been found, bisection refines the collision time using the supplied epsilon.

The epsilon therefore controls the precision of the final collision time rather than the initial trajectory
sampling.

## Limitations

The current collision system has several deliberate limitations.

### Fast-moving objects

The trajectory is sampled at fixed intervals. If an object moves through a sufficiently small collider
entirely between two samples, the collision may be missed.

For example:

```text
outside → collider → outside
```

can occur between two sampled points without either sample being inside the collider.

The current algorithm only detects a collision when a sample crosses from outside to inside.

A future implementation could address this through analytical intersection tests, adaptive subdivision,
continuous collision detection, or collider-specific swept-volume tests.

### Collision normals

As described above, the current collision normal is a movement-direction approximation rather than the actual
surface normal.

This is sufficient for detecting collisions but is not sufficient for robust physical collision response.

### Fixed sample count

The current `SAMPLE_COUNT` is fixed at 128.

This is simple and predictable, but it does not account for the speed of the object, the size of the collider,
or the complexity of the trajectory.

A future implementation could use adaptive sampling based on trajectory curvature, velocity, and collider
geometry.

## Error Handling

The collision system validates its numerical parameters.

The following conditions result in `std::invalid_argument`:

* Negative trajectory duration.
* Invalid AABB bounds.
* Negative circle radius.
* Non-positive root-finding epsilon.
* Root-finding lower bound greater than its upper bound.
* Non-positive collision epsilon.

These checks prevent invalid mathematical states from entering the collision algorithms.

## Extensibility

The system is designed so that new trajectory and collider implementations can be added independently.

For example, a new collider could provide:

```text
CapsuleCollider
PolygonCollider
OBBCollider
```

without requiring changes to the trajectory implementations.

Likewise, new trajectory types could represent:

```text
ProjectileTrajectory
SplineTrajectory
PhysicsTrajectory
RotationalTrajectory
```

without requiring changes to the collider implementations.

The primary requirement is that the implementations conform to the existing `Trajectory` and `Collider`
interfaces.

## Intended Architecture

The collision system knows **how to query movement and geometry**, but does not need to know how the
moving object was produced or how the collision should ultimately be handled.

Movement systems are responsible for creating trajectories.

Colliders are responsible for describing geometry.

The collision system is responsible for finding intersections between the two.

Collision response remains the responsibility of the higher-level physics or movement system.

## References

* [Device](Device.md)
* [Robot](Robot.md)
* [World](World.md)
* [Factory](Factory.md)
