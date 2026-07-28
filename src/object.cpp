#include <Object.hpp>

Transform::Transform() : position({0, 0}), rotation(0) {}
Transform::Transform(Vector2 vec, double rot) : position(vec), rotation(rot) {}

Object::Object(Transform transform, ObjectType type) : transform_(transform), type_(type) {}

Transform& Object::transform() {
    return transform_;
}

const Transform& Object::transform() const {
    return transform_;
}

ObjectType Object::getType() const noexcept {
    return type_;
}

char Object::toChar() const noexcept {
    return OBJECT_REPRESENTATION[static_cast<uint8_t>(type_)];
}