#pragma once

#include "../Device.hpp"

#include <algorithm>
#include <cmath>

class Motor : public Device {
public:
    /// @brief constructs the motor based off limits
    /// @param id 
    /// @param maxAngularVelocity Maximum shaft speed (rad/s) 
    /// @param maxAngularAcceleration Maximum acceleration (rad/s^2) 
    Motor(const std::vector<unsigned char>& id, double maxAngularVelocity, double maxAngularAcceleration);

    /**
     * Set motor output.
     * Range:
     *   -1.0 = full reverse
     *    0.0 = stopped
     *    1.0 = full forward
     */


    /// @brief Sets the motor output, -1.0 = full reverse, 0 = stop, 1.0 = full forward
    /// @param throttle 
    void setThrottle(double throttle);

    /// @brief gets the motor's current throttle
    /// @return the motor's current throttle
    double getThrottle() const noexcept;

    /// @brief gets the motor's current angular position
    /// @return the motor's current angular position
    double getAngularPosition() const noexcept;

    /// @brief gets the motor's current angular velocity 
    /// @return the motor's current angular velocity 
    double getAngularVelocity() const noexcept;

    /// @brief gets the motor's current angular acceleration
    /// @return the motor's current angular acceleration
    double getAngularAcceleration() const noexcept;

    /// @brief updates the motor based off delta time
    /// @param deltaTime 
    void update(long long deltaTime) override;

private:
    double throttle_;

    // State
    double angularPosition_;
    double angularVelocity_;
    double angularAcceleration_;

    // Limits
    double maxAngularVelocity_;
    double maxAngularAcceleration_;
};