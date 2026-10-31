#include <Devices/DifferentialDrive.hpp>

#include <World.hpp>
#include <Robot.hpp>

#include <cmath>
#include <algorithm>
#include <stdexcept>

DifferentialDriveTrajectory::DifferentialDriveTrajectory(
    Vector2 position, double rotation, double linearVelocity,
    double angularVelocity, double deltaTime
) : position_(position), rotation_(rotation), linearVelocity_(linearVelocity),
    angularVelocity_(angularVelocity), deltaTime_(deltaTime) 
{
    if (deltaTime < 0.0)
        throw std::invalid_argument("Trajectory delta time cannot be negative");
}

Vector2 DifferentialDriveTrajectory::position(double t) const {
    const double time = t * deltaTime_;
    const double theta = rotation_;
    const double omega = angularVelocity_;

    if (std::abs(omega) < 1e-9) {
        double x = position_.x + linearVelocity_ * std::cos(theta) * time;
        double y = position_.y + linearVelocity_ * std::sin(theta) * time;
        
        return {x, y};
    }

    const double radius = linearVelocity_ / omega;
    const double newTheta = theta + omega * time;

    return {
        position_.x + radius * (std::sin(newTheta) - std::sin(theta)),
        position_.y + radius * (std::cos(theta) - std::cos(newTheta))
    };
}

Vector2 DifferentialDriveTrajectory::velocity(double t) const {
    const double time = t * deltaTime_;
    const double theta = rotation_ + angularVelocity_ * time;

    return {
        linearVelocity_ * std::cos(theta),
        linearVelocity_ * std::sin(theta)
    };
}

double DifferentialDriveTrajectory::rotation(double t) const {
    return rotation_ + angularVelocity_ * deltaTime_ * t;
}

AABB DifferentialDriveTrajectory::bounds() const {
    constexpr double PI = 3.14159265358979323846;
    constexpr double TWO_PI = 2.0 * PI;
    constexpr double EPSILON = 1e-9;

    Vector2 minimum = position(0.0);
    Vector2 maximum = minimum;

    const auto include = [&](Vector2 point) {
        minimum.x = std::min(minimum.x, point.x);
        minimum.y = std::min(minimum.y, point.y);

        maximum.x = std::max(maximum.x, point.x);
        maximum.y = std::max(maximum.y, point.y);
    };

    const double theta0 = rotation_;
    const double deltaTheta = angularVelocity_ * deltaTime_;

    include(position(1.0));

    // Approximately straight-line movement.
    if (std::abs(angularVelocity_) < EPSILON) {
        return {minimum, maximum};
    }

    const double radius = linearVelocity_ / angularVelocity_;

    // A full revolution guarantees that the entire circle is traversed.
    if (std::abs(deltaTheta) >= TWO_PI) {
        const double circleRadius = std::abs(radius);

        const Vector2 center{
            position_.x - radius * std::sin(theta0),
            position_.y + radius * std::cos(theta0)
        };

        return {
            {center.x - circleRadius, center.y - circleRadius},
            {center.x + circleRadius, center.y + circleRadius}
        };
    }

    const double theta1 = theta0 + deltaTheta;

    const double minTheta = std::min(theta0, theta1);
    const double maxTheta = std::max(theta0, theta1);

    // Find every angle in the traversed interval matching baseAngle + k*pi.
    const auto includeExtrema = [&](double baseAngle) {
        const double firstK = std::ceil((minTheta - baseAngle) / PI);
        const double lastK = std::floor((maxTheta - baseAngle) / PI);

        for (double k = firstK; k <= lastK; k += 1.0) {
            const double theta = baseAngle + k * PI;
            const double t = (theta - theta0) / deltaTheta;

            if (t > 0.0 && t < 1.0) {
                include(position(t));
            }
        }
    };

    // X extrema occur when cos(theta) = 0.
    includeExtrema(PI / 2.0);

    // Y extrema occur when sin(theta) = 0.
    includeExtrema(0.0);

    return {minimum, maximum};
}

DifferentialDrive::DifferentialDrive(const std::string& id, const std::string& type) : 
            DifferentialDrive(id, "__undefined__", "__undefined__", 0.001, 0.001, type) {}

DifferentialDrive::DifferentialDrive(const std::string& id, const std::string& leftMotorId, const std::string& rightMotorId,
                                    double wheelRadius, double wheelBase, const std::string& type) : 
                                    Device(id, type), leftMotorId_(leftMotorId), rightMotorId_(rightMotorId),
                                    leftMotor_(nullptr), rightMotor_(nullptr), wheelRadius_(wheelRadius),
                                    wheelBase_(wheelBase), linearVelocity_(0.0), angularVelocity_(0.0) 
{
    if (wheelRadius <= 0.0)
        throw std::invalid_argument("Wheel radius must be greater than zero");

    if (wheelBase <= 0.0)
        throw std::invalid_argument("Wheel base must be greater than zero");
}

void DifferentialDrive::registerDifferentialDrive() {
    Device::Device_Factory.registerType<DifferentialDrive>("DifferentialDrive");
}

void DifferentialDrive::onAttach(Robot& robot) {
    Device::onAttach(robot);

    leftMotor_ = robot.getDevice<Motor>(leftMotorId_);
    rightMotor_ = robot.getDevice<Motor>(rightMotorId_);

    if (!leftMotor_)  
        throw std::runtime_error("DifferentialDrive: left motor '" + leftMotorId_ + "' was not found");

    if (!rightMotor_)
        throw std::runtime_error("DifferentialDrive: right motor '" + rightMotorId_ + "' was not found");
}

void DifferentialDrive::update(double deltaTime) {
    if (!robot_)
        throw std::runtime_error("DifferentialDrive is not attached to robot");

    if (!leftMotor_ || !rightMotor_)
        throw std::runtime_error("DifferentialDrive motors have not been initialized");

    // move this to the base update 
    if (deltaTime < 0)
        throw std::invalid_argument("deltaTime cannot be negative");

    const double dt = static_cast<double>(deltaTime);

    const double leftVelocity = leftMotor_->getAngularVelocity() * wheelRadius_;
    const double rightVelocity = rightMotor_->getAngularVelocity() * wheelRadius_;

    linearVelocity_ = (rightVelocity + leftVelocity) / 2.0;
    angularVelocity_ = (rightVelocity - leftVelocity) / wheelBase_;

    const Transform& transform = robot_->transform();
    
    DifferentialDriveTrajectory trajectory(
        transform.position, transform.rotation,
        linearVelocity_, angularVelocity_, dt
    );

    auto collision = robot_->world().cast(trajectory, *robot_);
    
    if (collision) {
        Transform newTransform = transform;
        
        double time = std::max(0.0, collision->time - 0.01);
        
        // gets the position and rotation just before the collision
        newTransform.position = trajectory.position(time);
        newTransform.rotation = trajectory.rotation(time);
        
        robot_->setTransform(newTransform);
        
        return;
    }
    
    Transform newTransform = transform;
    newTransform.position = trajectory.position(1.0);
    newTransform.rotation = trajectory.rotation(1.0);

    // Emit the device change before setting a new transform
    // for the robot so that the device changes before the robot
    // changes
    
    emitDeviceChange("differentialdrive.state", {
        {"device", id()}, 
        {"linear_velocity", linearVelocity_},
        {"angular_velocity", angularVelocity_}
    });
    
    robot_->setTransform(newTransform);
}

nlohmann::json DifferentialDrive::serialize() const {
    nlohmann::json json = Device::serialize();

    json["data"] = {
        {"LeftMotorId", leftMotorId_},
        {"RightMotorId", rightMotorId_},
        {"WheelRadius", wheelRadius_},
        {"WheelBase", wheelBase_},
        {"LinearVelocity", linearVelocity_},
        {"AngularVelocity", angularVelocity_}
    };

    return json;
}

void DifferentialDrive::deserialize(const nlohmann::json& json) {
    Device::deserialize(json);

    nlohmann::json data = json.at("data");

    leftMotorId_ = data.at("LeftMotorId");
    rightMotorId_ = data.at("RightMotorId");

    wheelRadius_ = data.at("WheelRadius");
    wheelBase_ = data.at("WheelBase");

    linearVelocity_ = data.at("LinearVelocity");
    angularVelocity_ = data.at("AngularVelocity");
}


double DifferentialDrive::getLinearVelicity() const noexcept {
    return linearVelocity_;
}

double DifferentialDrive::getAngularVelocity() const noexcept {
    return angularVelocity_;
}

int DifferentialDrive::updatePriority() const noexcept {
    return 50;
}