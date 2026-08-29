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

    // Ensure test directories exist for file testing
    std::filesystem::create_directories("assets/tests");

    {    
        World world(std::filesystem::path("assets/tests/DifferentialDriveConstruction.json"));

        // Fetch robot from its original static spawning cell
        Robot& robot = dynamic_cast<Robot&>(world.at({7, 10}));

        // Instantiating with max velocity 20 and max acceleration 40
        Motor& leftMotor = robot.addDevice<Motor>("LeftMotor_DD", 20, 40);
        Motor& rightMotor = robot.addDevice<Motor>("RightMotor_DD", 20, 40);

        // Drive setup: Wheel radius = 0.25, Track Width = 1.0
        DifferentialDrive& drive = robot.addDevice<DifferentialDrive>(
                "DD", "LeftMotor_DD", "RightMotor_DD", 0.25, 1.0
        );

        // Verify the devices can be retrieved after registration.
        assert(std::addressof(*robot.getDevice<Motor>("LeftMotor_DD")) == std::addressof(leftMotor));
        assert(std::addressof(*robot.getDevice<Motor>("RightMotor_DD")) == std::addressof(rightMotor));
        assert(std::addressof(*robot.getDevice<DifferentialDrive>("DD")) == std::addressof(drive));

        // Step 1: Straight line test (Symmetric Throttle)
        leftMotor.setThrottle(0.75);
        rightMotor.setThrottle(0.75);

        const Vector2 initialPosition = robot.transform().position;
        const double initialRotation = robot.transform().rotation;

        std::cout << world.toString() << "\n";

        // Process physics timestep via the parent robot container
        robot.update(1.0);

        const Vector2 firstPosition = robot.transform().position;
        const double firstRotation = robot.transform().rotation;

        // Equal motor speeds must produce NO angular rotation change
        assertNear(firstRotation, initialRotation);

        // The robot's spatial coordinates must change linearly
        assert(firstPosition.x != initialPosition.x || firstPosition.y != initialPosition.y);

        // Step 2: Turning Test (Asymmetric Throttle)
        rightMotor.setThrottle(-0.5); // Reverse right motor to force an explicit turn

        robot.update(1.0);

        const Vector2 finalPosition = robot.transform().position;
        const double finalRotation = robot.transform().rotation;

        // The robot must rotate because the wheels have mismatched thrust profiles
        assert(std::abs(finalRotation - firstRotation) > EPSILON);
        assert(finalPosition.x != firstPosition.x || finalPosition.y != firstPosition.y);

        // Explicitly update grid tracking inside your world structure before mapping grid assertions
        world.moveObject(world.at({11, 10}), {{5, 4}, 0}); 

        // Verify the updated grid handle matches the active robot address
        assert(std::addressof(world.at({5, 4})) == std::addressof(robot));

        // File serialization test
        world.saveToFile("assets/tests/ddSaveTest.json");
    }

    // Step 3: Deserialization and Persistence Verification
    World world(std::filesystem::path("assets/tests/ddSaveTest.json"));
    
    // Robot should persist at the grid position it was saved at ({5, 4})
    Robot& robot = dynamic_cast<Robot&>(world.at({5, 4}));

    Motor& leftMotor = *robot.getDevice<Motor>("LeftMotor_DD");
    Motor& rightMotor = *robot.getDevice<Motor>("RightMotor_DD");
    DifferentialDrive& drive = *robot.getDevice<DifferentialDrive>("DD");

    // Ensure values perfectly persisted through JSON cycle
    assertNear(leftMotor.getThrottle(), 0.75);
    assertNear(rightMotor.getThrottle(), -0.5);

    // Run the main update routine to test loop continuity
    robot.update(1.0);

    std::cout << "All DifferentialDrive test assertions passed successfully!" << std::endl;
    return 0;
}
