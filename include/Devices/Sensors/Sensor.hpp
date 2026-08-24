#pragma once

#include <Device.hpp>

/// @brief Base class for devices that measure properties of the simulation.
class Sensor : public Device {
public:
    virtual ~Sensor() = default;

    /// @brief Performs a sensor measurement.
    virtual void sense() = 0;

    void update(double deltaTime) override {
        sense();
    }
};