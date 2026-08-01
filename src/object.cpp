#include <Object.hpp>

Transform::Transform() : position({0, 0}), rotation(0) {}
Transform::Transform(Vector2 vec, double rot) : position(vec), rotation(rot) {}
Transform::Transform(uint32_t x, uint32_t y, double rot) : position({x, y}), rotation(rot) {}

Object::Object() : world_(nullptr), transform_(Transform()), name_("") {}

Object::Object(World& world, Transform transform, const std::string& name) : world_(&world), transform_(transform), name_(name) {}

Object::~Object() = default;

Transform& Object::transform() {
    return transform_;
}

const Transform& Object::transform() const {
    return transform_;
}

World& Object::world() {
    return *world_;
}

const World& Object::world() const {
    return *world_;
}

std::string& Object::name() {
    return name_;
}

const std::string& Object::name() const {
    return name_;
}