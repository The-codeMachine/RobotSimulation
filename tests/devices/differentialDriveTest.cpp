#include <Registration.hpp>
#include <World.hpp>
#include <Robot.hpp>
#include <Devices/DifferentialDrive.hpp>

#include <cassert>
#include <iostream>

int main() {
    registerBuiltinObjects();

    World world(std::filesystem::path("assets/tests/differentialDriveConstruction.json"));
    Robot& robot = dynamic_cast<Robot&>(world.at({5, 5}));

    Motor& leftMotor = robot.addDevice<Motor>("LeftMotor_DD", 100, 100);
    Motor& rightMotor = robot.addDevice<Motor>("RightMotor_DD", 100, 100);
    DifferentialDrive& drive = robot.addDevice<DifferentialDrive>("DD", "LeftMotor_DD", "RightMotor_DD", 4, 10);

    leftMotor.setThrottle(1.0);
    rightMotor.setThrottle(1.0);

    robot.update(1.5);

    assert(std::addressof(world.at({6, 5})) == std::addressof(robot));

    rightMotor.setThrottle(-1.0);

    robot.update(8);

    assert(std::addressof(world.at({5, 4})) == std::addressof(robot));
}