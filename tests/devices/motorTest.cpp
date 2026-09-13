#include <Registration.hpp>

#include <World.hpp>
#include <Devices/Motor.hpp>

#include <cassert>
#include <cmath>
#include <iostream>

namespace {
    constexpr double EPSILON = 1e-9;

    void assertNear(double actual, double expected, double epsilon = EPSILON) {
        assert(std::abs(actual - expected) <= epsilon);
    }

    void testInitialState(Robot& robot) {
        Motor& motor = dynamic_cast<Motor&>(robot.addDevice(std::make_unique<Motor>("init_motor")));

        assertNear(motor.getThrottle(), 0.0);
        assertNear(motor.getAngularPosition(), 0.0);
        assertNear(motor.getAngularVelocity(), 0.0);
        assertNear(motor.getAngularAcceleration(), 0.0);
    }

    void testThrottleClamping(Robot& robot) {
        Motor& motor = dynamic_cast<Motor&>(robot.addDevice(std::make_unique<Motor>("throttle_clamping_motor", 20.0, 10.0)));

        motor.setThrottle(0.5);
        assertNear(motor.getThrottle(), 0.5);

        motor.setThrottle(2.0);
        assertNear(motor.getThrottle(), 1.0);

        motor.setThrottle(-2.0);
        assertNear(motor.getThrottle(), -1.0);

        motor.setThrottle(0.0);
        assertNear(motor.getThrottle(), 0.0);
    }

    void testAccelerationLimit(Robot& robot) {
        Motor& motor = dynamic_cast<Motor&>(robot.addDevice(std::make_unique<Motor>("accel_limit_motor", 20.0, 5.0)));

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

    void testAccelerationTowardsTarget(Robot& robot) {
        Motor& motor = dynamic_cast<Motor&>(robot.addDevice(std::make_unique<Motor>("accel_tt_motor", 20.0, 10.0)));

        motor.setThrottle(0.5);

        // Target velocity = 10 rad/s.
        motor.update(1);

        // Acceleration is limited to 10 rad/s^2.
        // Therefore velocity reaches 10 rad/s after 1 second.
        assertNear(motor.getAngularAcceleration(), 10.0);
        assertNear(motor.getAngularVelocity(), 10.0);
    }

    void testVelocityLimit(Robot& robot) {
        Motor& motor = dynamic_cast<Motor&>(robot.addDevice(std::make_unique<Motor>("velocity_limit_motor", 20.0, 100.0)));

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

    void testPositionIntegration(Robot& robot) {
        Motor& motor = dynamic_cast<Motor&>(robot.addDevice(std::make_unique<Motor>("pos_integration_motor", 20.0, 100.0)));

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

    void testReverseMotion(Robot& robot) {
        Motor& motor = dynamic_cast<Motor&>(robot.addDevice(std::make_unique<Motor>("reverse_motor", 20.0, 10.0)));

        motor.setThrottle(-1.0);

        motor.update(1);

        assertNear(motor.getAngularAcceleration(), -10.0);
        assertNear(motor.getAngularVelocity(), -10.0);

        motor.update(1);

        assertNear(motor.getAngularVelocity(), -20.0);
    }

    void testStopping(Robot& robot) {
        Motor& motor = dynamic_cast<Motor&>(robot.addDevice(std::make_unique<Motor>("stopping_motor", 20.0, 10.0)));

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

    void testFullStop(Robot& robot) {
        Motor& motor = dynamic_cast<Motor&>(robot.addDevice(std::make_unique<Motor>("full_stop_motor", 20.0, 10.0)));

        motor.setThrottle(1.0);
        motor.update(1);

        motor.setThrottle(0.0);

        // Takes one second to decelerate from 10 -> 0
        // at -10 rad/s^2.
        motor.update(1);

        assertNear(motor.getAngularVelocity(), 0.0);
        assertNear(motor.getAngularAcceleration(), -10.0);
    }

    void testWorldSave(World& world) {
        world.saveToFile("assets/tests/motorSaveWorld.json");
        World w(std::filesystem::path("assets/tests/motorSaveWorld.json"));

        Robot& robot = dynamic_cast<Robot&>(world.at({5, 5}));

        // Does not test that everything was saved, not a full test
        // but it should be enough to know whether or not it saved

        assert(robot.getDevice<Motor>("init_motor")->getThrottle() == 0.0);
        assert(robot.getDevice<Motor>("throttle_clamping_motor")->getThrottle() == 0.0);
        assert(robot.getDevice<Motor>("accel_limit_motor")->getThrottle() == 1.0);
        assert(robot.getDevice<Motor>("accel_tt_motor")->getThrottle() == 0.5);
        assert(robot.getDevice<Motor>("velocity_limit_motor")->getThrottle() == 1.0);
        assert(robot.getDevice<Motor>("pos_integration_motor")->getThrottle() == 0.5);
        assert(robot.getDevice<Motor>("reverse_motor")->getThrottle() == -1.0);
        assert(robot.getDevice<Motor>("full_stop_motor")->getThrottle() == 0.0);
    }
}

int main() {
    try {
        registerBuiltinObjects();
        
        World world(std::filesystem::path("assets/tests/motorWorld.json"));
        Robot& robot = dynamic_cast<Robot&>(world.at({5, 5}));

        testInitialState(robot);
        testThrottleClamping(robot);
        testAccelerationLimit(robot);
        testAccelerationTowardsTarget(robot);
        testVelocityLimit(robot);
        testPositionIntegration(robot);
        testReverseMotion(robot);
        testStopping(robot);
        testFullStop(robot);
        testWorldSave(world);
    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        return -1;
    }

    return 0;
}