#include <Registration.hpp>
#include <World.hpp>
#include <Robot.hpp>
#include <Devices/Motor.hpp>
#include <Devices/DifferentialDrive.hpp>
#include <Devices/Sensors/Sensor.hpp>
#include <Devices/Sensors/SensorShape.hpp>
#include <Collision.hpp>

#include <array>
#include <cmath>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <numbers>
#include <string>

namespace {

constexpr double PI = std::numbers::pi;

void printTransform(const Transform& transform) {
    std::cout << "(" << std::fixed << std::setprecision(2)
              << transform.position.x << ", "
              << transform.position.y << ")  "
              << transform.rotation << " rad";
}

void demonstrateGeometry() {
    std::cout << "\n[geometry]\n";

    const Vector2 a{3.0, 4.0};
    const Vector2 b{1.0, 2.0};

    std::cout << "Vector length: " << Vector2::length(a) << "\n";
    std::cout << "Dot product: " << Vector2::dot(a, b) << "\n";

    CircleCollider circle({5.0, 5.0}, 2.0);
    AABBCollider box({8.0, 4.0}, {10.0, 6.0});

    std::cout << "Circle contains (5, 6): "
              << std::boolalpha << circle.contains({5.0, 6.0}) << "\n";
    std::cout << "AABB contains (9, 5): "
              << std::boolalpha << box.contains({9.0, 5.0}) << "\n";

    const auto root = findRoot(
        [](double x) { return x * x - 2.0; },
        0.0,
        2.0
    );

    if (root)
        std::cout << "Root finder: sqrt(2) = " << *root << "\n";
}

void demonstrateSensorShapes() {
    std::cout << "\n[sensor shapes]\n";

    const Transform origin({0.0, 0.0}, 0.0);
    const Transform point({4.0, 0.0}, 0.0);

    SensorShapeBall ball(origin, 5.0);
    SensorShapeCone cone(origin, 90.0, 5.0);

    const std::array<Transform, 4> vertices{
        Transform({-2.0, -2.0}, 0.0),
        Transform({ 2.0, -2.0}, 0.0),
        Transform({ 2.0,  2.0}, 0.0),
        Transform({-2.0,  2.0}, 0.0)
    };

    SensorShapeQuadratic quadratic(vertices);

    std::cout << "Ball contains point: " << ball.contains(point) << "\n";
    std::cout << "Cone contains point: " << cone.contains(point) << "\n";
    std::cout << "Quadratic contains (1, 1): "
              << quadratic.contains(Transform({1.0, 1.0}, 0.0)) << "\n";
}

} // namespace

int main() {
    try {
        // Register the built-in factories before loading the world.
        registerBuiltinObjects();

        std::cout << "RobotSimulationLib\n";
        std::cout << "=================\n";

        // Load a complete simulation world from JSON.
        World world(std::filesystem::path("assets/tests/introWorld.json"));

        Robot& robot = dynamic_cast<Robot&>(world.at({3.0, 6.0}));

        // Add a differential-drive robot entirely through the public API.
        Motor& leftMotor = robot.addDevice<Motor>(
            "left", 20.0, 40.0
        );

        Motor& rightMotor = robot.addDevice<Motor>(
            "right", 20.0, 40.0
        );

        DifferentialDrive& drive = robot.addDevice<DifferentialDrive>(
            "drive",
            "left",
            "right",
            0.25,   // wheel radius
            1.00    // wheel base
        );

        // The camera is attached locally to the robot and uses a cone shape.
        ViewSensor& camera = robot.addDevice<ViewSensor>(
            "camera",
            90.0,
            8.0,
            Transform({0.5, 0.0}, 0.0)
        );

        std::cout << "\n[world]\n";
        std::cout << world.toString() << "\n";

        std::cout << "\n[robot]\n";
        std::cout << "Initial position: ";
        printTransform(robot.transform());
        std::cout << "\n";

        // Drive forward for two seconds.
        leftMotor.setThrottle(0.75);
        rightMotor.setThrottle(0.75);

        constexpr double DT = 0.1;
        constexpr int STEPS = 20;

        for (int i = 0; i < STEPS; ++i)
            robot.update(DT);

        std::cout << "After driving:    ";
        printTransform(robot.transform());
        std::cout << "\n";
        std::cout << "Linear velocity:  " << drive.getLinearVelicity() << "\n";
        std::cout << "Angular velocity: " << drive.getAngularVelocity() << "\n";

        // Ask the camera what it can see.
        std::cout << "\n[vision]\n";
        const Image& image = camera.image();
        std::cout << "Detections: " << image.size() << "\n";

        for (const Detection& detection : image.detections()) {
            std::cout << "  - " << detection.object->name() << " at ";
            printTransform(detection.object->transform());
            std::cout << "\n";
        }

        // Demonstrate the generic accelerated trajectory and collision system.
        std::cout << "\n[collision]\n";

        AcceleratedTrajectory probe(
            {3.0, 6.0},
            {5.0, 0.0},
            {0.0, 0.0},
            2.0
        );

        const Object& robotObject = robot;
        const auto collision = world.cast(probe, robotObject);

        if (collision) {
            std::cout << "Collision detected at t = "
                      << collision->time << "\n";
            std::cout << "Collision position: ";
            printTransform(Transform(collision->position, 0.0));
            std::cout << "\n";
            std::cout << "Hit object: " << collision->object->name() << "\n";
        } else {
            std::cout << "No collision detected.\n";
        }

        demonstrateGeometry();
        demonstrateSensorShapes();

        // Serialize the complete world and write it to disk.
        const std::filesystem::path output = "introWorldSave.json";
        world.saveToFile(output);

        std::cout << "\n[serialization]\n";
        std::cout << "World saved to " << output.string() << "\n";
        std::cout << "Reloading saved world...\n";

        World restored(output);
        std::cout << "Reload successful.\n";
        std::cout << restored.toString() << "\n";

        std::filesystem::remove(output);

        std::cout << "\nDemo complete.\n";
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Demo failed: " << e.what() << '\n';
        return 1;
    }
}
