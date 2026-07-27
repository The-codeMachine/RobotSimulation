#include <Object.hpp>

Object::Object() : transform_{0, 0}, type_(ObjectType::EMPTY) {}

Object::Object(Transform transform, ObjectType type) : transform_(transform), type_(type) {}

Transform& Object::getTransform() {
    return transform_;
}

Transform Object::getTransform() const noexcept {
    return transform_;
}

ObjectType Object::getType() const noexcept {
    return type_;
}

char Object::toChar() const noexcept {
    return OBJECT_REPRESENTATION[static_cast<uint8_t>(type_)];
}