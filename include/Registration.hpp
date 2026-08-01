#pragma once

#include "Object.hpp"
#include "Robot.hpp"

/// @brief Registers all builtin objects. Must be done before constructing a world
void registerBuiltinObjects() {
    Robot::registerRobot();
}