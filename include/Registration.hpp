#pragma once

#include <Collision.hpp>
#include <Object.hpp>
#include <Robot.hpp>

#include <Devices/Motor.hpp>
#include <Devices/DifferentialDrive.hpp>

/// @brief Registers all builtin objects. Must be done before constructing a world
void registerBuiltinObjects() {
    AABBCollider::registerAABBCollider();
    CircleCollider::registerCircleCollider();
    
    Motor::registerMotor();
    DifferentialDrive::registerDifferentialDrive();

    Robot::registerRobot();
    Empty::registerEmpty();
    Wall::registerWall();
}