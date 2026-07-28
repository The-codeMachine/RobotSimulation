#include <Object.hpp>

Transform::Transform() : position({0, 0}), rotation(0) {}
Transform::Transform(Vector2 vec, double rot) : position(vec), rotation(rot) {}
Transform::Transform(uint32_t x, uint32_t y, double rot) : position({x, y}), rotation(rot) {}

Object::Object() : world_(nullptr), transform_(Transform()), type_(ObjectType::EMPTY) {}

Object::Object(World& world, Transform transform, ObjectType type) : world_(&world), transform_(transform), type_(type) {}

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

ObjectType Object::getType() const noexcept {
    return type_;
}

char Object::toChar() const noexcept {
    return OBJECT_REPRESENTATION[static_cast<uint8_t>(type_)];
}