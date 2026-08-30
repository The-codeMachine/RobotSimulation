#include <Registration.hpp>
#include <World.hpp>
#include <Robot.hpp>
#include <Devices/Motor.hpp>
#include <Devices/DifferentialDrive.hpp>
#include <Devices/Sensors/Sensor.hpp>

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace {

constexpr double MIN_THROTTLE = -1.0;
constexpr double MAX_THROTTLE = 1.0;
constexpr double DEFAULT_STEP = 0.1;

double clampThrottle(double value) {
    return std::clamp(value, MIN_THROTTLE, MAX_THROTTLE);
}

void printTransform(const Transform& transform) {
    std::cout << std::fixed << std::setprecision(2)
              << "(" << transform.position.x
              << ", " << transform.position.y
              << ")  " << transform.rotation << " rad";
}

void printHelp() {
    std::cout << R"(
Commands
--------
help
    Show this help message.

status
    Show the robot's position, rotation, and current motor throttles.

throttle <left> <right>
    Set the left and right motor throttles.
    Values are clamped to [-1.0, 1.0].

adjust <left_delta> <right_delta>
    Adjust the current motor throttles by the given amounts.
    Example: adjust 0.25 -0.10

forward <amount>
    Increase both motor throttles by <amount>.
    Example: forward 0.25

backward <amount>
    Decrease both motor throttles by <amount>.
    Example: backward 0.25

turn <amount>
    Adjust the motor throttles in opposite directions.
    Positive values turn one way; negative values turn the other.
    Example: turn 0.25

stop
    Set both motor throttles to zero.

step <seconds>
    Advance the simulation by the specified amount of time.
    Example: step 0.10

run <seconds> [dt]
    Advance the simulation for <seconds>.
    Optionally specify the timestep.
    Example: run 2.0 0.10

vision
    Show the objects currently detected by the robot's visual sensor.

world
    Print the current world representation.

quit
    Exit the program.

Tip:
    A nice interactive sequence is:
        throttle 0.5 0.5
        run 2
        turn 0.3
        run 1
        stop
        vision
)";

}

void printStatus(const Robot& robot, double leftThrottle, double rightThrottle) {
    std::cout << "\n[robot]\n";
    std::cout << "Position:  ";
    printTransform(robot.transform());
    std::cout << "\n";
    std::cout << "Left throttle:  " << std::fixed << std::setprecision(2)
              << leftThrottle << "\n";
    std::cout << "Right throttle: " << std::fixed << std::setprecision(2)
              << rightThrottle << "\n";
}

void printVision(const ViewSensor& camera) {
    const Image& image = camera.image();

    std::cout << "\n[vision]\n";
    std::cout << "Detections: " << image.size() << "\n";

    for (const Detection& detection : image.detections()) {
        if (detection.object == nullptr)
            continue;

        std::cout << "  - " << detection.object->name() << " at ";
        printTransform(detection.object->transform());
        std::cout << "\n";
    }
}

void applyThrottle(
    Motor& leftMotor,
    Motor& rightMotor,
    double& leftThrottle,
    double& rightThrottle,
    double left,
    double right
) {
    leftThrottle = clampThrottle(left);
    rightThrottle = clampThrottle(right);

    leftMotor.setThrottle(leftThrottle);
    rightMotor.setThrottle(rightThrottle);
}

bool parseDouble(std::istringstream& stream, double& value) {
    return static_cast<bool>(stream >> value);
}

} // namespace

int main() {
    try {
        registerBuiltinObjects();

        // Keep the world path relative to the working directory, matching
        // the existing intro example.
        const std::filesystem::path worldPath =
            "assets/tests/introWorld.json";

        World world(worldPath);

        Robot& robot = dynamic_cast<Robot&>(world.at({3.0, 6.0}));

        Motor& leftMotor = robot.addDevice<Motor>(
            "left", 20.0, 40.0
        );

        Motor& rightMotor = robot.addDevice<Motor>(
            "right", 20.0, 40.0
        );

        robot.addDevice<DifferentialDrive>(
            "drive",
            "left",
            "right",
            0.25,   // wheel radius
            1.00    // wheel base
        );

        ViewSensor& camera = robot.addDevice<ViewSensor>(
            "camera",
            90.0,   // field of view
            8.0,    // range
            Transform({0.5, 0.0}, 0.0)
        );

        double leftThrottle = 0.0;
        double rightThrottle = 0.0;

        applyThrottle(
            leftMotor,
            rightMotor,
            leftThrottle,
            rightThrottle,
            0.0,
            0.0
        );

        std::cout << "RobotSimulationLib Interactive Demo\n";
        std::cout << "====================================\n";
        std::cout << "World: " << worldPath.string() << "\n";
        std::cout << "Type 'help' for commands.\n";

        std::string line;

        while (true) {
            std::cout << "\n> ";

            if (!std::getline(std::cin, line))
                break;

            std::istringstream commandStream(line);
            std::string command;
            commandStream >> command;

            // Make commands case-insensitive without modifying arguments.
            std::transform(
                command.begin(),
                command.end(),
                command.begin(),
                [](unsigned char c) {
                    return static_cast<char>(std::tolower(c));
                }
            );

            if (command.empty())
                continue;

            if (command == "help") {
                printHelp();
            }
            else if (command == "status") {
                printStatus(robot, leftThrottle, rightThrottle);
            }
            else if (command == "vision") {
                printVision(camera);
            }
            else if (command == "world") {
                std::cout << "\n[world]\n";
                std::cout << world.toString() << "\n";
            }
            else if (command == "stop") {
                applyThrottle(
                    leftMotor,
                    rightMotor,
                    leftThrottle,
                    rightThrottle,
                    0.0,
                    0.0
                );

                std::cout << "Motors stopped.\n";
            }
            else if (command == "throttle") {
                double left;
                double right;

                if (!parseDouble(commandStream, left) ||
                    !parseDouble(commandStream, right)) {
                    std::cout << "Usage: throttle <left> <right>\n";
                    continue;
                }

                applyThrottle(
                    leftMotor,
                    rightMotor,
                    leftThrottle,
                    rightThrottle,
                    left,
                    right
                );

                std::cout << "Throttle set to left="
                          << leftThrottle
                          << ", right="
                          << rightThrottle << "\n";
            }
            else if (command == "adjust") {
                double leftDelta;
                double rightDelta;

                if (!parseDouble(commandStream, leftDelta) ||
                    !parseDouble(commandStream, rightDelta)) {
                    std::cout
                        << "Usage: adjust <left_delta> <right_delta>\n";
                    continue;
                }

                applyThrottle(
                    leftMotor,
                    rightMotor,
                    leftThrottle,
                    rightThrottle,
                    leftThrottle + leftDelta,
                    rightThrottle + rightDelta
                );

                std::cout << "Throttle adjusted to left="
                          << leftThrottle
                          << ", right="
                          << rightThrottle << "\n";
            }
            else if (command == "forward") {
                double amount;

                if (!parseDouble(commandStream, amount)) {
                    std::cout << "Usage: forward <amount>\n";
                    continue;
                }

                applyThrottle(
                    leftMotor,
                    rightMotor,
                    leftThrottle,
                    rightThrottle,
                    leftThrottle + amount,
                    rightThrottle + amount
                );

                std::cout << "Moving throttle forward by "
                          << amount << ".\n";
            }
            else if (command == "backward") {
                double amount;

                if (!parseDouble(commandStream, amount)) {
                    std::cout << "Usage: backward <amount>\n";
                    continue;
                }

                applyThrottle(
                    leftMotor,
                    rightMotor,
                    leftThrottle,
                    rightThrottle,
                    leftThrottle - amount,
                    rightThrottle - amount
                );

                std::cout << "Moving throttle backward by "
                          << amount << ".\n";
            }
            else if (command == "turn") {
                double amount;

                if (!parseDouble(commandStream, amount)) {
                    std::cout << "Usage: turn <amount>\n";
                    continue;
                }

                // Positive amount: increase left / decrease right.
                // Negative amount: increase right / decrease left.
                applyThrottle(
                    leftMotor,
                    rightMotor,
                    leftThrottle,
                    rightThrottle,
                    leftThrottle + amount,
                    rightThrottle - amount
                );

                std::cout << "Turn adjustment: "
                          << amount << ".\n";
            }
            else if (command == "step") {
                double seconds;

                if (!parseDouble(commandStream, seconds) ||
                    seconds <= 0.0) {
                    std::cout << "Usage: step <positive_seconds>\n";
                    continue;
                }

                robot.update(seconds);

                std::cout << "Advanced simulation by "
                          << seconds << " seconds.\n";
                printStatus(robot, leftThrottle, rightThrottle);
            }
            else if (command == "run") {
                double seconds;
                double dt = DEFAULT_STEP;

                if (!parseDouble(commandStream, seconds) ||
                    seconds <= 0.0) {
                    std::cout << "Usage: run <positive_seconds> [dt]\n";
                    continue;
                }

                if (commandStream >> dt && dt <= 0.0) {
                    std::cout << "dt must be positive.\n";
                    continue;
                }

                double remaining = seconds;

                while (remaining > 0.0) {
                    const double stepSize = std::min(dt, remaining);
                    robot.update(stepSize);
                    remaining -= stepSize;
                }

                std::cout << "Advanced simulation by "
                          << seconds << " seconds.\n";
                printStatus(robot, leftThrottle, rightThrottle);
            }
            else if (command == "quit" ||
                     command == "exit") {
                std::cout << "Exiting.\n";
                break;
            }
            else {
                std::cout << "Unknown command: " << command
                          << ". Type 'help' for available commands.\n";
            }
        }

        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Interactive demo failed: "
                  << e.what() << '\n';
        return 1;
    }
}
