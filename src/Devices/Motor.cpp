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
    // Convert milliseconds to seconds
    double dt = static_cast<double>(deltaTime) / 1000.0;

    /*
        Calculate desired velocity.

        Example:
        power = 0.5
        max velocity = 20 rad/s

        target = 10 rad/s
    */
    double targetVelocity = throttle_ * maxAngularVelocity_;


    /*
        Calculate required acceleration.

        a = (vf - vi) / t
    */
    double requiredAcceleration = (targetVelocity - angularVelocity_) / dt;


    /*
        Motors cannot accelerate infinitely.

        Clamp acceleration.
    */
    angularAcceleration_ = std::clamp(requiredAcceleration, -maxAngularAcceleration_, maxAngularAcceleration_);


    /*
        Update velocity.

        v = v0 + at
    */
    angularVelocity_ += angularAcceleration_ * dt;


    /*
        Prevent exceeding motor speed.
    */
    angularVelocity_ = std::clamp(angularVelocity_, -maxAngularVelocity_, maxAngularVelocity_);


    /*
        Update rotation.

        θ = θ + ωt
    */
    angularPosition_ += angularVelocity_ * dt;
}