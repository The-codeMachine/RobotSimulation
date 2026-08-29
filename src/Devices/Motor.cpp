#include <Devices/Motor.hpp>

Motor::Motor(const std::string& id, const std::string& type) 
    : Device(id, type), throttle_(0.0), angularPosition_(0.0),
      angularVelocity_(0.0), angularAcceleration_(0.0), maxAngularVelocity_(100.0),
      maxAngularAcceleration_(100.0)
    {}

Motor::Motor(const std::string& id, double maxAngularVelocity, double maxAngularAcceleration, const std::string& type) 
    : Device(id, type), throttle_(0.0), angularPosition_(0.0),
      angularVelocity_(0.0), angularAcceleration_(0.0), maxAngularVelocity_(maxAngularVelocity),
      maxAngularAcceleration_(maxAngularAcceleration)
    {}

nlohmann::json Motor::serialize() const {
    nlohmann::json json = Device::serialize();
    
    json["type"] = "Motor";

    json["data"] = {
        {"angular_position", angularPosition_},
        {"angular_velocity", angularVelocity_},
        {"angular_acceleration", angularAcceleration_},
        {"max_angular_velocity", maxAngularVelocity_},
        {"max_angular_acceleration", maxAngularAcceleration_},
        {"throttle", throttle_}
    };

    return json;
}

void Motor::deserialize(const nlohmann::json& json) {
    nlohmann::json data = json.at("data");

    angularPosition_        = data.at("angular_position");
    angularVelocity_        = data.at("angular_velocity");
    angularAcceleration_    = data.at("angular_acceleration");

    maxAngularVelocity_     = data.at("max_angular_velocity");
    maxAngularAcceleration_ = data.at("max_angular_acceleration");

    throttle_               = data.at("throttle");
}

void Motor::setThrottle(double power) {
    throttle_ = std::clamp(power, -1.0, 1.0);
}

double Motor::getThrottle() const noexcept {
    return throttle_;
}

double Motor::getAngularPosition() const noexcept {
    return angularPosition_;
}

double Motor::getAngularVelocity() const noexcept {
    return angularVelocity_;
}

double Motor::getAngularAcceleration() const noexcept {
    return angularAcceleration_;
}

void Motor::update(double deltaTime) {
    if (deltaTime < 0.0)
        throw std::invalid_argument("Delta time cannot be negative");

    if (deltaTime == 0.0)
        return;

    const double targetVelocity = throttle_ * maxAngularVelocity_;
    const double velocityDiff = targetVelocity - angularVelocity_;

    if (std::abs(velocityDiff) > 1e-9) {
        // Determine acceleration direction based on the target velocity
        angularAcceleration_ = (velocityDiff > 0.0) ? maxAngularAcceleration_ : -maxAngularAcceleration_;

        // Calculate the exact time needed to reach target velocity
        double timeToTarget = velocityDiff / angularAcceleration_;

        // If it reaches target velocity BEFORE deltaTime ends, split the step
        if (timeToTarget < deltaTime) {
            const double initialVelocity = angularVelocity_;

            // Phase 1: Accelerate up to the target velocity
            angularPosition_ += initialVelocity * timeToTarget + 0.5 * angularAcceleration_ * timeToTarget * timeToTarget;
            angularVelocity_ = targetVelocity;

            // Phase 2: Cruise at target velocity for the remaining time
            const double remainingTime = deltaTime - timeToTarget;
            angularPosition_ += angularVelocity_ * remainingTime;
            
            // Acceleration stops once target is reached
            angularAcceleration_ = 0.0; 
        } 
        // Otherwise, it accelerates for the entire duration of deltaTime
        else {
            const double initialVelocity = angularVelocity_;
            angularPosition_ += initialVelocity * deltaTime + 0.5 * angularAcceleration_ * deltaTime * deltaTime;
            angularVelocity_ += angularAcceleration_ * deltaTime;
        }
    } 
    // Already at target velocity: Constant velocity motion
    else {
        angularAcceleration_ = 0.0;
        angularPosition_ += angularVelocity_ * deltaTime;
    }

    // Keep numerical safety clamp against floating-point drift
    angularVelocity_ = std::clamp(angularVelocity_, -maxAngularVelocity_, maxAngularVelocity_);
}
