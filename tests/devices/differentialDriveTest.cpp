#include <World.hpp>
#include <Robot.hpp>
#include <Devices/DifferentialDrive.hpp>

#include <cassert>
#include <iostream>

int main() {
    World world(std::filesystem::path("assests/test/differentialDriveConstruction.json"));
    Robot& robot = dynamic_cast<Robot&>(world.at({5, 5}));
    
    Motor& leftMotor = robot.addDevice<Motor>("LeftMotor_DD", 100, 100);
    Motor& rightMotor = robot.addDevice<Motor>("RightMotor_DD", 100, 100);
    DifferentialDrive& drive = robot.addDevice<DifferentialDrive>("DD", "LeftMotor_DD", "RightMotor_DD", 4, 10);

    leftMotor.setThrottle(1.0);
    rightMotor.setThrottle(1.0);

    robot.update(10);

    std::cout << world.toString() << "\n";
}