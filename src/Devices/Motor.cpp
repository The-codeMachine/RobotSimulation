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
        {"max_angular_acceleration", maxAngularAcceleration_}
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

    const double targetVelocity =
        throttle_ * maxAngularVelocity_;

    const double requiredAcceleration =
        (targetVelocity - angularVelocity_) / deltaTime;

    angularAcceleration_ =
        std::clamp(
            requiredAcceleration,
            -maxAngularAcceleration_,
            maxAngularAcceleration_
        );

    // Preserve the velocity at the beginning of the timestep.
    const double initialVelocity = angularVelocity_;

    // θ = θ₀ + v₀t + ½at²
    angularPosition_ +=
        initialVelocity * deltaTime +
        0.5 * angularAcceleration_ * deltaTime * deltaTime;

    // v = v₀ + at
    angularVelocity_ +=
        angularAcceleration_ * deltaTime;

    // Prevent numerical overshoot.
    angularVelocity_ =
        std::clamp(
            angularVelocity_,
            -maxAngularVelocity_,
            maxAngularVelocity_
        );
}
