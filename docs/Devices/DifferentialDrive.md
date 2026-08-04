# Differential Drive

This is the differential-drive movement device. It moves `Robot` around by converting two `Motor`s' angular
velocities into linear/angular velocities. These linear/angular velocities are applied to the `Robot` and adjust its
transform accordingly.

The `Motor`s it references must be attached **before** attaching this device. If they are not, it will fail to
attach and will throw an exception. This device does not own the `Motor`s it references. Rather, it stores
pointers to the `Motor`s the robot owns, based on the specified ID.

The update priority of this device is `50`. This is so `Motor`s are updated before it, allowing it to apply their
current velocities rather than their previous velocities.

This device uses collisions to calculate its trajectory throughout the world based on the motors' acceleration
and velocity. It uses a `Trajectory` implementation called `DifferentialDriveTrajectory`.

## Movement

A differential drive uses two independently controlled wheels:

* **Left wheel** — controls the velocity of the left side of the robot.
* **Right wheel** — controls the velocity of the right side of the robot.

The angular velocity of each motor is converted into the corresponding wheel's linear velocity using the wheel
radius:

```text
v_left  = ω_left  × r
v_right = ω_right × r
```

where:

* `ω_left` is the left motor's angular velocity.
* `ω_right` is the right motor's angular velocity.
* `r` is the wheel radius.

The robot's overall linear and angular velocity are then calculated as:

```text
v = (v_right + v_left) / 2

ω = (v_right - v_left) / b
```

where `b` is the wheel base, or the distance between the two wheels.

The resulting `v` and `ω` are used to construct a `DifferentialDriveTrajectory`.

### Straight-line movement

If both wheels have the same velocity, the robot does not rotate:

```text
v_left = v_right
        ↓
ω = 0
```

The robot therefore travels in a straight line in its current direction.

### Rotation

If the wheels have different velocities, the robot follows a curved path.

If the wheels move in opposite directions, the resulting linear velocity can be zero while the angular
velocity is non-zero. This causes the robot to rotate in place.

The direction of rotation is determined by:

```text
ω = (v_right - v_left) / b
```

Therefore:

* `v_right > v_left` rotates the robot counter-clockwise.
* `v_left > v_right` rotates the robot clockwise.

The exact direction follows the coordinate system used by `Robot` and `Transform`.

## Updating

When `update()` is called, the device performs the following operations:

1. Verify that the device is attached to a `Robot`.
2. Verify that both referenced motors have been initialized.
3. Read the angular velocities of both motors.
4. Convert the motor angular velocities into wheel linear velocities.
5. Calculate the robot's linear and angular velocity.
6. Construct a `DifferentialDriveTrajectory` representing the movement for the current timestep.
7. Cast that trajectory through the `World` to determine whether a collision occurs.
8. If a collision occurs, move the robot to the collision position and rotate it to the corresponding point
   along the trajectory.
9. Otherwise, move the robot to the end of the trajectory.

The device does **not** directly calculate collision geometry itself. Collision detection is delegated to
`World::cast()`.

If a collision occurs, the robot is currently moved to the collision point and its rotation is updated to the
rotation at that point. Collision response is not currently implemented.

## Differential Drive Trajectory

`DifferentialDriveTrajectory` represents the continuous path taken by a differential-drive robot during a single
physics timestep.

It stores:

* The starting position.
* The starting rotation.
* The linear velocity.
* The angular velocity.
* The duration of the timestep.

The trajectory uses a normalized time parameter `t`. A value of:

```text
t = 0
```

represents the beginning of the timestep, while:

```text
t = 1
```

represents the end of the timestep.

The actual elapsed time is calculated as:

```text
time = t × deltaTime
```

This allows the trajectory to be queried at any point during the timestep.

### Position

When the angular velocity is effectively zero, the trajectory is a straight line:

```text
x(t) = x₀ + v cos(θ) t
y(t) = y₀ + v sin(θ) t
```

where `θ` is the robot's starting rotation.

When the angular velocity is non-zero, the robot follows a circular arc. The radius of this arc is:

```text
R = v / ω
```

The position is calculated using:

```text
x(t) = x₀ + R [sin(θ + ωt) - sin(θ)]

y(t) = y₀ + R [cos(θ) - cos(θ + ωt)]
```

This provides a continuous representation of the robot's movement rather than approximating the movement as a
series of straight-line steps.

### Velocity

The trajectory can also return the robot's instantaneous velocity at any point in time.

The robot's orientation at time `t` is:

```text
θ(t) = θ₀ + ωt
```

The velocity vector is then:

```text
v_x = v cos(θ(t))
v_y = v sin(θ(t))
```

This means the velocity vector continuously changes direction while the robot is turning.

### Rotation

The rotation at any point in the trajectory is:

```text
θ(t) = θ₀ + ωt
```

The trajectory therefore provides both the position and orientation necessary for continuous collision
detection.

## Collision Detection

The complete `DifferentialDriveTrajectory` is passed to `World::cast()` rather than only checking the robot's
final position.

This is important because a robot can collide with an object somewhere along its curved path even if its final
position is not inside the object.

For example, a robot turning around an obstacle may have a trajectory that intersects the obstacle before reaching
the end of its timestep. The collision system can identify this point and return the normalized collision time.

When a collision is detected, `DifferentialDrive::update()` uses that time to calculate the robot's rotation:

```text
rotation = trajectory.rotation(collisionTime)
```

and places the robot at the collision position.

Currently, this only prevents the robot from continuing past the collision. A proper collision response system
may be implemented later.

## Serialization

The device serializes the following values:

* `LeftMotorId` — ID of the left motor.
* `RightMotorId` — ID of the right motor.
* `WheelRadius` — radius of both wheels.
* `WheelBase` — distance between the wheels.
* `LinearVelocity` — current calculated linear velocity.
* `AngularVelocity` — current calculated angular velocity.

The motor references are stored as IDs rather than serialized motor objects because the
`DifferentialDrive` does not own those motors.

During deserialization, the IDs and configuration values are restored. The motor pointers are resolved again when
the device is attached to a `Robot`.

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

* [Collisions](../Collisions.md)
* [Device](../Device.md)
* [Motor](../Motor.md)
* [Robot](../Robot.md)
* [World](../World.md)
