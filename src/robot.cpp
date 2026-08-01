#include <Robot.hpp>

Robot::Robot(World& world, Transform t) : Object(world, t, "Robot") {}

void Robot::registerRobot() {
    Object::Object_Factory.registerType<Robot>("Robot");
}

void Robot::update(long long deltaTime) {
    for (auto& d : devices_) 
        d->update(deltaTime);
}