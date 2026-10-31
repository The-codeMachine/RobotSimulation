# Differential Drive

This is the differential-drive movement device. It moves a `Robot` by converting two `Motor`s' angular velocities
into linear and angular velocity.

The `Motor`s it references must be attached **before** attaching this device. If they are not, attachment fails
with an exception. This device does not own the `Motor`s it references. Instead, it stores pointers to the
motors owned by the robot, resolved using their IDs.

The update priority of this device is `50`. This allows the `Motor`s to update first, so the differential drive
uses their current velocities rather than their previous velocities.

The device uses a `DifferentialDriveTrajectory` to represent movement over the current physics timestep and
passes that trajectory to `World::cast()` for collision detection.

`DifferentialDrive`s are moveable and copyable.

## Movement

A differential drive uses two independently controlled wheels:

- **Left wheel** — controls the velocity of the left side of the robot.
- **Right wheel** — controls the velocity of the right side of the robot.

The angular velocity of each motor is converted into wheel linear velocity using the wheel radius:

```text
v_left  = ω_left  × r
v_right = ω_right × r
```

where `ω_left` and `ω_right` are the motor angular velocities and `r` is the wheel radius.

The robot's linear and angular velocities are:

```text
v = (v_right + v_left) / 2

ω = (v_right - v_left) / b
```

where `b` is the wheel base, or the distance between the wheels.

### Straight-line movement

If both wheels have the same velocity, the robot does not rotate:

```text
v_left = v_right
        ↓
ω = 0
```

The robot travels in a straight line in its current direction.

### Rotation

If the wheels have different velocities, the robot follows a curved path. If the wheels move in opposite
directions, the linear velocity can be zero while angular velocity is non-zero, causing the robot to rotate in
place.

The direction follows:

```text
ω = (v_right - v_left) / b
```

Therefore:

- `v_right > v_left` produces counter-clockwise rotation.
- `v_left > v_right` produces clockwise rotation.

The exact interpretation depends on the coordinate system used by `Robot` and `Transform`.

## Updating

When `update()` is called, the device:

1. Verifies that it is attached to a `Robot`.
2. Verifies that both referenced motors have been initialized.
3. Reads the angular velocities of both motors.
4. Converts motor angular velocities into wheel linear velocities.
5. Calculates the robot's linear and angular velocity.
6. Constructs a `DifferentialDriveTrajectory` for the current timestep.
7. Calls `robot_->world().cast(trajectory, *robot_)`.
8. If a collision is reported, moves the robot to a point just before the reported impact.
9. Otherwise, moves the robot to the trajectory's final position and rotation.

Collision geometry is not calculated by `DifferentialDrive` itself. `World::cast()` performs broad-phase
candidate selection, and the candidate collider performs the narrow-phase cast.

### Collision positioning

When a collision is reported, the current implementation backs off from the collision time before updating the
robot's transform. Conceptually:

```cpp
double time = std::max(0.0, collision->time - 0.01);

newTransform.position = trajectory.position(time);
newTransform.rotation = trajectory.rotation(time);
```

The value `0.01` is an arbitrary offset in **normalized trajectory time**, not a fixed physical distance. Its
physical effect changes with timestep duration and movement speed. It should eventually be replaced with a
configurable, scale-aware tolerance or a distance-based backoff.

Collision response is not currently implemented. The current behavior stops the robot just before the reported
collision; it does not calculate sliding, bouncing, or a physically resolved post-impact velocity.

## Differential Drive Trajectory

`DifferentialDriveTrajectory` represents the continuous reference-point path of a differential-drive robot during
one physics timestep.

It stores:

- Starting position
- Starting rotation
- Linear velocity
- Angular velocity
- Timestep duration (`deltaTime`)

The trajectory uses normalized time `t` in the interval `[0, 1]`:

- `t = 0` represents the beginning of the timestep.
- `t = 1` represents the end of the timestep.

The corresponding elapsed time is:

```text
τ = deltaTime × t
```

Using `τ` explicitly is important: velocity is measured per unit of real time, while `t` is dimensionless.

### Position

Let `x₀` and `y₀` be the starting position, `θ₀` the starting rotation, `v` the linear velocity, and `ω` the
angular velocity.

When angular velocity is effectively zero, the trajectory is a straight line:

```text
x(t) = x₀ + v cos(θ₀) × deltaTime × t
y(t) = y₀ + v sin(θ₀) × deltaTime × t
```

When angular velocity is non-zero, the robot follows a circular arc. Its signed turning radius is:

```text
R = v / ω
```

The position is:

```text
θ(t) = θ₀ + ω × deltaTime × t

x(t) = x₀ + R [sin(θ(t)) - sin(θ₀)]

y(t) = y₀ + R [cos(θ₀) - cos(θ(t))]
```

This gives a continuous path rather than approximating the motion as a sequence of straight-line steps.

### Velocity

The instantaneous velocity vector is:

```text
v_x(t) = v cos(θ(t))
v_y(t) = v sin(θ(t))
```

where:

```text
θ(t) = θ₀ + ω × deltaTime × t
```

These are physical velocity components. If an implementation instead exposes the derivative with respect to
normalized time, that derivative includes an additional `deltaTime` factor.

### Rotation

The rotation at normalized time `t` is:

```text
θ(t) = θ₀ + ω × deltaTime × t
```

The trajectory therefore provides the position and orientation needed by the movement and collision systems.

## Trajectory Bounds

`DifferentialDriveTrajectory::bounds()` returns an axis-aligned bounding box (AABB) covering the trajectory's
reference point over the full normalized interval `[0, 1]`.

The bounds are calculated analytically rather than by fixed-step sampling. The starting and ending positions are
included, along with every coordinate extremum that lies within the angular interval.

For non-zero angular velocity, the path is a circular arc:

```text
x(t) = x₀ + (v / ω) [sin(θ(t)) - sin(θ₀)]

y(t) = y₀ + (v / ω) [cos(θ₀) - cos(θ(t))]
```

The coordinate extrema occur when the derivative of that coordinate is zero:

| Coordinate | Extremum condition |
|---|---|
| X | `cos(θ) = 0` |
| Y | `sin(θ) = 0` |

The bounds calculation follows this approach:

1. Initialize the minimum and maximum using the starting position.
2. Include the final position.
3. Handle approximately straight-line movement separately.
4. If the angular interval covers a full revolution, use the AABB of the complete circular path.
5. Otherwise, find each relevant X and Y extremum within the angular interval, evaluate the trajectory there, and
   expand the bounds.

This captures the mathematical extrema of the reference-point arc, subject to floating-point precision.

**Important:** these bounds do not automatically account for the physical dimensions of the robot's collider.
The broad phase can miss collisions unless the moving object's collider size is included in a conservative swept
bound or otherwise accounted for by the spatial query.

## Collision Detection

The complete trajectory is passed to `World::cast()` rather than checking only the robot's final position. This
allows the world to consider objects in the trajectory's swept region, including objects that the robot could
encounter before the timestep ends.

`World::cast()` uses the trajectory AABB to find candidate objects in `map_`. Each candidate's collider then
performs the narrow-phase test through its `cast()` implementation. If multiple collisions are returned, the
world selects the one with the earliest normalized collision time.

The broad phase and narrow phase have different responsibilities:

- The AABB broad phase reduces the number of colliders that need to be tested.
- The collider's narrow-phase cast determines whether and when the trajectory intersects its geometry.

The accuracy of the overall result therefore depends on both conservative candidate bounds and the correctness
of the collider's cast algorithm. Exact trajectory bounds alone do not guarantee exact collision detection.

## Input Validation

Trajectory constructors must reject non-finite numerical inputs, not merely negative durations. Comparisons such
as `deltaTime < 0.0` do not reject NaN, and infinity can also propagate into trajectory calculations.

The differential-drive trajectory should validate that its position components, rotation, linear velocity,
angular velocity, and timestep duration are finite, and that `deltaTime` is non-negative. Invalid parameters
should result in `std::invalid_argument`.

For example:

```cpp
if (!std::isfinite(position.x) ||
    !std::isfinite(position.y) ||
    !std::isfinite(rotation) ||
    !std::isfinite(linearVelocity) ||
    !std::isfinite(angularVelocity) ||
    !std::isfinite(deltaTime) ||
    deltaTime < 0.0) {
    throw std::invalid_argument(
        "Invalid differential drive trajectory parameters"
    );
}
```

The same finite-value requirement should be applied consistently to other trajectory constructors and their
inputs.

## Serialization

The device serializes:

- `LeftMotorId` — ID of the left motor.
- `RightMotorId` — ID of the right motor.
- `WheelRadius` — radius of both wheels.
- `WheelBase` — distance between the wheels.
- `LinearVelocity` — current calculated linear velocity.
- `AngularVelocity` — current calculated angular velocity.

Motor references are stored as IDs rather than serialized motor objects because the `DifferentialDrive` does not
own the motors.

During deserialization, IDs and configuration values are restored. Motor pointers are resolved again when the
device is attached to a `Robot`.

## Configuration

The constructor requires:

```text
DifferentialDrive(
    id,
    leftMotorId,
    rightMotorId,
    wheelRadius,
    wheelBase,
    type
)
```

`wheelRadius` and `wheelBase` must both be greater than zero. A non-positive value causes
`std::invalid_argument` to be thrown.

The device also provides a simplified constructor that creates a device with undefined motor IDs and default
wheel dimensions. This constructor is primarily useful when the object must be created before its serialized
configuration is loaded.

## References

- [Collisions](../Collisions.md)
- [Device](../Device.md)
- [Motor](../Motor.md)
- [Robot](../Robot.md)
- [World](../World.md)
