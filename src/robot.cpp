#include <Robot.hpp>

Robot::Robot(World& world, Transform t) : Object(world, t, ObjectType::ROBOT) {}

void Robot::update(long long deltaTime) {
    for (auto& d : devices_) 
        d->update(deltaTime);
}