#include <Object.hpp>

Transform::Transform() : position({0, 0}), rotation(0) {}
Transform::Transform(Vector2 vec, double rot) : position(vec), rotation(rot) {}
Transform::Transform(uint32_t x, uint32_t y, double rot) : position({x, y}), rotation(rot) {}
Transform::Transform(const nlohmann::json& json) : position({json.at("x"), json.at("y")}), rotation(json.at("rotation")) {}

nlohmann::json Transform::serialize() const {
    nlohmann::json json;

    json["x"] = position.x;
    json["y"] = position.y;
    json["rotation"] = rotation;

    return json;
}

Object::Object() : world_(nullptr), transform_(Transform()), name_("") {}

Object::Object(World& world, Transform transform, const std::string& name) : world_(&world), transform_(transform), name_(name) {}

Object::~Object() = default;

nlohmann::json Object::serialize() const {
    return {};
}

void Object::deserialize(const nlohmann::json& json) {

}

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