#include <Devices/DifferentialDrive.hpp>

#include <Robot.hpp>

#include <cmath>
#include <stdexcept>

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

void DifferentialDrive::onAttach(Robot& robot) {
    Device::onAttach(robot);

    leftMotor_ = robot.getDevice<Motor>(leftMotorId_);
    rightMotor_ = robot.getDevice<Motor>(rightMotorId_);

    if (!leftMotor_)  
        throw std::runtime_error("DifferentialDrive: left motor '" + leftMotorId_ + "' was not found");

    if (!rightMotor_)
        throw std::runtime_error("DifferentialDrive: right motor '" + rightMotorId_ + "' was not found");
}

void DifferentialDrive::update(long long deltaTime) {
    if (!robot_)
        throw std::runtime_error("DifferentialDrive is not attached to robot");

    if (!leftMotor_ || !rightMotor_)
        throw std::runtime_error("DifferentialDrive motors have not been initialized");

    // move this to the base update 
    if (deltaTime < 0)
        throw std::invalid_argument("deltaTime cannot be negative");

    const double dt = static_cast<double>(deltaTime) / 1000.0;

    const double leftVelocity = leftMotor_->getAngularVelocity() * wheelRadius_;
    const double rightVelocity = rightMotor_->getAngularAcceleration() * wheelRadius_;

    linearVelocity_ = (rightVelocity + leftVelocity) / 2.0;
    angularVelocity_ = (rightVelocity - leftVelocity) / wheelBase_;

    Transform& transform = robot_->transform();
    
    const double theta = transform.rotation;

    if (std::abs(angularVelocity_) < 1e-9) {
        // straight line movement
        transform.position.x += linearVelocity_ * std::cos(theta) * dt;
        transform.position.y += linearVelocity_ * std::sin(theta) * dt;
    } else {
        // follows in a circular arc
        const double radius = linearVelocity_ / angularVelocity_;
        const double newTheta  = theta + angularVelocity_ * dt;

        transform.position.x += radius * (std::sin(newTheta) - std::sin(theta));
        transform.position.y += radius * (-std::cos(newTheta) + std::cos(theta));

        transform.rotation = newTheta;
    } 
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