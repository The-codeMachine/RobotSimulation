#include <Devices/Motor.hpp>

#include <cassert>
#include <cmath>
#include <iostream>

namespace {
    constexpr double EPSILON = 1e-9;

    void assertNear(double actual, double expected, double epsilon = EPSILON) {
        assert(std::abs(actual - expected) <= epsilon);
    }

    void testInitialState() {
        Motor motor("motor");

        assertNear(motor.getThrottle(), 0.0);
        assertNear(motor.getAngularPosition(), 0.0);
        assertNear(motor.getAngularVelocity(), 0.0);
        assertNear(motor.getAngularAcceleration(), 0.0);
    }

    void testThrottleClamping() {
        Motor motor("motor", 20.0, 10.0);

        motor.setThrottle(0.5);
        assertNear(motor.getThrottle(), 0.5);

        motor.setThrottle(2.0);
        assertNear(motor.getThrottle(), 1.0);

        motor.setThrottle(-2.0);
        assertNear(motor.getThrottle(), -1.0);

        motor.setThrottle(0.0);
        assertNear(motor.getThrottle(), 0.0);
    }

    void testAccelerationLimit() {
        Motor motor("motor", 20.0, 5.0);

        motor.setThrottle(1.0);

        motor.update(0.1);

        // Target velocity = 20 rad/s.
        // Required acceleration = 200 rad/s^2.
        // Motor is limited to 5 rad/s^2.
        //
        // velocity = 0 + 5 * 0.1 = 0.5 rad/s
        assertNear(motor.getAngularAcceleration(), 5.0);
        assertNear(motor.getAngularVelocity(), 0.5);
    }

    void testAccelerationTowardsTarget() {
        Motor motor("motor", 20.0, 10.0);

        motor.setThrottle(0.5);

        // Target velocity = 10 rad/s.
        motor.update(1);

        // Acceleration is limited to 10 rad/s^2.
        // Therefore velocity reaches 10 rad/s after 1 second.
        assertNear(motor.getAngularAcceleration(), 10.0);
        assertNear(motor.getAngularVelocity(), 10.0);
    }

    void testVelocityLimit() {
        Motor motor("motor", 20.0, 100.0);

        motor.setThrottle(1.0);

        // Required acceleration is 20 rad/s^2,
        // which is below the 100 rad/s^2 limit.
        motor.update(1);

        assertNear(motor.getAngularVelocity(), 20.0);

        // Continue accelerating. Velocity must not exceed
        // the maximum velocity.
        motor.update(1);

        assertNear(motor.getAngularVelocity(), 20.0);
    }

    void testPositionIntegration() {
        Motor motor("motor", 20.0, 100.0);

        motor.setThrottle(0.5);

        // After 1 second:
        // velocity = 10 rad/s
        // position = 10 rad
        motor.update(1);

        assertNear(motor.getAngularVelocity(), 10.0);
        assertNear(motor.getAngularPosition(), 9.5);

        // Another second at 10 rad/s:
        // position = 20 rad
        motor.update(1);
        
        assertNear(motor.getAngularVelocity(), 10.0);
        assertNear(motor.getAngularPosition(), 19.5);
    }

    void testReverseMotion() {
        Motor motor("motor", 20.0, 10.0);

        motor.setThrottle(-1.0);

        motor.update(1);

        assertNear(motor.getAngularAcceleration(), -10.0);
        assertNear(motor.getAngularVelocity(), -10.0);

        motor.update(1);

        assertNear(motor.getAngularVelocity(), -20.0);
    }

    void testStopping() {
        Motor motor("motor", 20.0, 10.0);

        motor.setThrottle(1.0);

        motor.update(1);

        assertNear(motor.getAngularVelocity(), 10.0);

        // Stop the motor.
        motor.setThrottle(0.0);

        motor.update(0.5);

        // Required acceleration:
        // (0 - 10) / 0.5 = -20 rad/s^2
        //
        // Limited to -10 rad/s^2.
        //
        // New velocity:
        // 10 + (-10 * 0.5) = 5 rad/s
        assertNear(motor.getAngularAcceleration(), -10.0);
        assertNear(motor.getAngularVelocity(), 5.0);
    }

    void testFullStop() {
        Motor motor("motor", 20.0, 10.0);

        motor.setThrottle(1.0);
        motor.update(1);

        motor.setThrottle(0.0);

        // Takes one second to decelerate from 10 -> 0
        // at -10 rad/s^2.
        motor.update(1);

        assertNear(motor.getAngularVelocity(), 0.0);
        assertNear(motor.getAngularAcceleration(), -10.0);
    }
}

int main() {
    testInitialState();
    testThrottleClamping();
    testAccelerationLimit();
    testAccelerationTowardsTarget();
    testVelocityLimit();
    testPositionIntegration();
    testReverseMotion();
    testStopping();
    testFullStop();

    return 0;
}