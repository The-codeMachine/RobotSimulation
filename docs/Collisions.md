# Collision System

The collision system provides the geometric and mathematical primitives required to determine whether a moving
object intersects a collider during a physics timestep.

The system separates collision detection into two phases:

- **Broad phase:** quickly identifies objects that could collide with the moving object.
- **Narrow phase:** performs the actual geometric collision test.

The broad phase uses axis-aligned bounding boxes (AABBs) and the world's spatial map. The narrow phase delegates
the geometric test to the candidate collider's `cast()` implementation.

The collision system does not own the objects or colliders involved in a query. It performs collision queries
and returns a `CollisionResult` describing a detected collision.

## Architecture

Movement systems describe motion using a `Trajectory`. The trajectory provides its position and bounds over the
normalized timestep. `World::cast()` uses those bounds to select candidate objects, then asks each candidate
collider to perform its narrow-phase cast.

```text
Trajectory
    |
    v
Calculate swept AABB
    |
    v
World::cast()
    |
    v
Find candidate objects using map_
    |
    v
Collider::cast()
    |
    v
CollisionResult
(earliest collision)
```

This keeps movement mathematics, world-level spatial lookup, and collider geometry separate:

- Movement systems create trajectories.
- The trajectory reports its motion and reference-point bounds.
- The world selects candidate objects.
- Colliders perform narrow-phase geometric tests.
- The movement or physics system is responsible for collision response.

## Axis-Aligned Bounding Boxes

`AABB` represents an axis-aligned bounding box using its minimum and maximum corners:

```cpp
struct AABB {
    Vector2 minimum;
    Vector2 maximum;

    bool intersects(const AABB& other) const noexcept {
        return minimum.x <= other.maximum.x
            && maximum.x >= other.minimum.x
            && minimum.y <= other.maximum.y
            && maximum.y >= other.minimum.y;
    }
};
```

The `intersects()` comparisons are inclusive. Boxes that only touch at an edge or corner are therefore considered
to intersect.

This `AABB` is a general bounds primitive used by the broad phase. It is distinct from `AABBCollider`, which is
a collider implementation with point-containment and signed-distance behavior.

## Trajectories

A `Trajectory` represents movement over one physics timestep. Its time parameter is normalized to `[0, 1]`:

- `t = 0` is the beginning of the timestep.
- `t = 1` is the end of the timestep.

The interface exposes the position, velocity, and bounds:

```cpp
class Trajectory {
public:
    virtual ~Trajectory() = default;

    virtual Vector2 position(double t) const = 0;
    virtual Vector2 velocity(double t) const = 0;
    virtual AABB bounds() const = 0;
};
```

`bounds()` returns an AABB covering the trajectory's reference point over the entire normalized interval
`[0, 1]`. This allows `World::cast()` to perform broad-phase lookup without knowing the mathematical form of a
particular trajectory.

Different movement systems can provide different trajectory implementations while remaining compatible with
the same world-level casting interface.

## Accelerated Trajectory

`AcceleratedTrajectory` represents motion with constant acceleration. It stores the initial position, initial
velocity, constant acceleration, and timestep duration.

Using normalized time `t` and timestep duration `deltaTime`, the position is:

\[
p(t)=p_0+v_0\Delta t\,t+\frac12a(\Delta t\,t)^2
\]

The extrema of each coordinate occur where that coordinate's velocity is zero. For a coordinate with initial
velocity component `v₀` and acceleration component `a`, the candidate extremum is:

\[
t=-\frac{v_0}{a\Delta t}
\]

The bounds calculation evaluates:

- The initial position at `t = 0`.
- The final position at `t = 1`.
- Any X-coordinate extremum strictly inside `[0, 1]`.
- Any Y-coordinate extremum strictly inside `[0, 1]`.

The resulting AABB is the exact mathematical bound of the reference-point trajectory, subject to floating-point
precision. If the relevant acceleration component is zero, that coordinate has no interior velocity-zero
extremum to evaluate.

## Differential Drive Trajectory

`DifferentialDriveTrajectory` represents a differential-drive robot moving along a straight line or circular arc.

Let `v` be linear velocity, `ω` angular velocity, `deltaTime` the timestep duration, and `θ₀` the starting
rotation. The angle at normalized time `t` is:

\[
\theta(t)=\theta_0+\omega\Delta t\,t
\]

When angular velocity is non-zero, the reference point follows:

\[
x(t)=x_0+\frac{v}{\omega}[\sin(\theta(t))-\sin(\theta_0)]
\]

\[
y(t)=y_0+\frac{v}{\omega}[\cos(\theta_0)-\cos(\theta(t))]
\]

The coordinate extrema occur when:

| Coordinate | Extremum condition |
|---|---|
| X | `cos(θ) = 0` |
| Y | `sin(θ) = 0` |

The analytical bounds implementation:

1. Initializes the bounds using the starting position.
2. Includes the final position.
3. Handles approximately straight-line movement separately.
4. Handles a full revolution by using the complete circle's AABB.
5. For a partial revolution, finds every relevant coordinate extremum inside the angular interval.
6. Evaluates the trajectory at those extrema and expands the bounds.

This avoids fixed-step sampling when calculating the trajectory AABB. As with the accelerated trajectory, the result
is a mathematical bound of the reference point, subject to floating-point precision.

## Broad-Phase Candidate Selection

`World::cast()` obtains the trajectory AABB and converts its coordinates into grid-cell ranges:

```cpp
const AABB bounds = trajectory.bounds();

const int minX = static_cast<int>(std::floor(bounds.minimum.x));
const int maxX = static_cast<int>(std::floor(bounds.maximum.x));
const int minY = static_cast<int>(std::floor(bounds.minimum.y));
const int maxY = static_cast<int>(std::floor(bounds.maximum.y));
```

It visits the covered integer coordinates and looks each one up in `map_`. Missing cells are skipped. Null
objects, the explicitly ignored object, and empty objects are also skipped.

Each remaining object is a broad-phase candidate. Its collider is asked to perform the narrow-phase cast.
`World::cast()` retains the collision with the smallest normalized time.

This avoids scanning every object in `map_` for every cast.

### Spatial assumptions and limitations

The current map-based query relies on these assumptions:

- World positions are integer grid coordinates.
- Each collider fits within its object's grid cell.
- The trajectory AABB conservatively covers the moving object's collision geometry.
- Collider bounds, trajectory coordinates, and map positions use the same world coordinate system.

A trajectory's `bounds()` currently describes the reference point. It does not automatically expand the bounds
by the dimensions of the moving object's collider. Consequently, a robot with a non-zero collider size needs a
conservative swept bound that includes that size; otherwise the broad phase can omit a cell containing a real
collision.

Likewise, if a stationary candidate collider can extend outside its object's grid cell, looking up only cells
inside the trajectory AABB can miss it. Supporting arbitrary-sized colliders requires conservative query
expansion or a secondary multi-cell spatial index.

## Narrow-Phase Collider Casting

The narrow phase is responsible for determining whether the trajectory intersects a candidate collider and when
the earliest collision occurs. `World::cast()` delegates this work to the collider's `cast()` implementation.

The world does not need to know whether the collider is an AABB, circle, or another geometric shape. New collider
types can provide their own cast implementation without changing the world's broad-phase lookup.

The narrow-phase algorithm must correctly handle the geometry and motion it claims to support. In particular,
fixed-step sampling can miss tangential contact or a fast pass-through if the trajectory enters and exits a
collider between samples. An exact reference-point AABB does not fix such a narrow-phase limitation.

The earlier signed-distance sampling and bisection approach is one possible narrow-phase strategy, but it is not
equivalent to an analytical or otherwise guaranteed continuous intersection test. If the current
`Collider::cast()` implementation uses fixed sampling, its sampling limitations still apply.

## Collision Result

A successful cast returns a `CollisionResult`. The world-level query compares results by normalized collision
time and returns the earliest one.

The collision time is expressed in the trajectory's normalized interval `[0, 1]`. The collision position should
correspond to the trajectory evaluated at that time:

```cpp
collisionPosition = trajectory.position(collisionTime);
```

The precise fields and normal calculation are determined by the `CollisionResult` and collider implementation.

## Collision Response

Collision detection and collision response are separate responsibilities.

The collision system reports a collision; it does not decide how the moving object should react. A movement or
physics system may stop the object, slide it, bounce it, or apply another response.

A movement system that backs away from the collision time should not treat an arbitrary normalized-time offset as
a fixed physical distance. For example, subtracting `0.01` from normalized time has a different physical effect
depending on timestep duration and speed. A configurable scale-aware tolerance or distance-based backoff is a
better long-term approach.

## Numerical Validation

Trajectory constructors should reject non-finite values as well as invalid ranges. A check such as
`deltaTime < 0.0` does not reject NaN, and infinite values can propagate into bounds and collision calculations.

For a differential-drive trajectory, the position components, rotation, linear velocity, angular velocity, and
timestep duration should be finite, and `deltaTime` should be non-negative. Invalid parameters should cause
`std::invalid_argument` to be thrown.

Equivalent finite-value validation should be applied to all trajectory types and to numerical parameters used by
collider casts and root-finding routines.

## Outstanding Work

The following items should be addressed before treating the collision system as reliable:

| Priority | Item | Reason |
|---|---|---|
| Critical | Include the moving object's collider size in swept bounds | Prevents broad-phase false negatives |
| Critical | Verify the grid-cell and collider-size assumptions | Prevents large colliders from being omitted |
| High | Replace the arbitrary `0.01` normalized-time backoff | Makes collision positioning scale-aware |
| High | Validate finite values in every trajectory constructor | Prevents invalid numerical values propagating |
| High | Test tangential collisions and fast pass-through | Exercises narrow-phase cases that fixed sampling can miss |
| Medium | Add world-space collider bounds | Enables more reliable broad-phase rejection and query expansion |
| Medium | Test zero angular velocity and full revolutions | Covers important differential-drive bounds edge cases |

A particularly important distinction is that exact mathematical bounds for a robot's reference-point trajectory do
not automatically provide exact collision detection. Reliable results require both conservative broad-phase
bounds that account for collider geometry and a narrow-phase cast that correctly detects the relevant
intersections.

## Extensibility

The architecture allows trajectory and collider implementations to be added independently.

Potential collider implementations include:

- `CapsuleCollider`
- `PolygonCollider`
- `OBBCollider`

Potential trajectory implementations include:

- `ProjectileTrajectory`
- `SplineTrajectory`
- `PhysicsTrajectory`
- `RotationalTrajectory`

New implementations must conform to the relevant interfaces and provide bounds or geometric tests that satisfy
the assumptions of the broad- and narrow-phase algorithms.

## Moveability and Copyability

All currently implemented `Collision`s and `Trajectory`s are fully moveable and copyable.

## References

- [Device](Device.md)
- [Robot](Robot.md)
- [World](World.md)
- [Factory](Factory.md)
- [Vector2](Vector2.md)
