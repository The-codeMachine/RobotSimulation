#include <Robot.hpp>

Robot::Robot(World& world, Transform t) : world_(&world), transform_(t) {}

Transform& Robot::transform() {
    return transform_;
}

const Transform& Robot::transform() const {
    return transform_;
}

World& Robot::world() {
    return *world_;
}

const World& Robot::world() const {
    return *world_;
}

void Robot::update(long long deltaTime) {
    for (auto& d : devices_) 
        d->update(deltaTime);
}