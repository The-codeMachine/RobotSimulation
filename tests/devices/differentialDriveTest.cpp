#include <Registration.hpp>
#include <World.hpp>
#include <Robot.hpp>
#include <Devices/DifferentialDrive.hpp>

#include <cassert>
#include <cmath>
#include <filesystem>
#include <iostream>
#include <memory>

namespace {

constexpr double EPSILON = 1e-9;

void assertNear(double actual, double expected, double epsilon = EPSILON) {
    assert(std::abs(actual - expected) < epsilon);
}

void assertVectorNear(const Vector2& actual, const Vector2& expected,
                      double epsilon = EPSILON) {
    assertNear(actual.x, expected.x, epsilon);
    assertNear(actual.y, expected.y, epsilon);
}

} // namespace

int main() {
    registerBuiltinObjects();

    {    
        World world(std::filesystem::path("assets/tests/differentialDriveConstruction.json"));

        Robot& robot = dynamic_cast<Robot&>(world.at({5, 5}));

        Motor& leftMotor = robot.addDevice<Motor>("LeftMotor_DD", 100, 100);

        Motor& rightMotor = robot.addDevice<Motor>("RightMotor_DD", 100, 100);

        DifferentialDrive& drive = robot.addDevice<DifferentialDrive>(
                "DD", "LeftMotor_DD", "RightMotor_DD", 4, 10
        );

        // Verify the devices can be retrieved after registration.
        assert(std::addressof(*robot.getDevice<Motor>("LeftMotor_DD")) == std::addressof(leftMotor));
        assert(std::addressof(*robot.getDevice<Motor>("RightMotor_DD")) == std::addressof(rightMotor));
        assert(std::addressof(*robot.getDevice<DifferentialDrive>("DD")) == std::addressof(drive));

        leftMotor.setThrottle(1.0);
        rightMotor.setThrottle(1.0);

        const Vector2 initialPosition = robot.transform().position;
        const double initialRotation = robot.transform().rotation;

        robot.update(1.5);

        const Vector2 firstPosition = robot.transform().position;
        const double firstRotation = robot.transform().rotation;

        // Equal motor speeds should produce no angular velocity.
        assertNear(firstRotation, initialRotation);

        // The robot should have moved.
        assert(firstPosition != initialPosition);

        // It should still occupy the expected world cell.
        assert(std::addressof(world.at({6, 5})) == std::addressof(robot));

        rightMotor.setThrottle(-1.0);

        robot.update(8.0);

        const Vector2 finalPosition = robot.transform().position;
        const double finalRotation = robot.transform().rotation;

        // The robot should have rotated because the wheels now have
        // different velocities.
        assert(std::abs(finalRotation - firstRotation) > EPSILON);

        // The robot should have moved again.
        assert(finalPosition != firstPosition);

        // Verify the resulting world location.
        assert(std::addressof(world.at({5, 4})) == std::addressof(robot));

        // file saving
        world.saveToFile("assets/tests/ddSaveTest.json");
    }

    // file loading

    World world(std::filesystem::path("assets/tests/ddSaveTest.json"));
    Robot& robot = dynamic_cast<Robot&>(world.at({5, 4}));

    Motor& leftMotor = *robot.getDevice<Motor>("LeftMotor_DD");
    Motor& rightMotor = *robot.getDevice<Motor>("RightMotor_DD");
    DifferentialDrive& drive = *robot.getDevice<DifferentialDrive>("DD");

    drive.update(10);

    assert(std::addressof(world.at({5, 5})) == std::addressof(robot));

}