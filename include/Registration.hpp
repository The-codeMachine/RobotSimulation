#pragma once

#include <Object.hpp>
#include <Robot.hpp>
#include <Devices/Motor.hpp>

/// @brief Registers all builtin objects. Must be done before constructing a world
void registerBuiltinObjects() {
    Motor::registerMotor();
    Robot::registerRobot();
}