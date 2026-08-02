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

Object::Object(World& world, Transform transform, const std::string& name, char glyph) : world_(&world), transform_(transform), name_(name), glyph_(glyph) {}

Object::~Object() = default;

nlohmann::json Object::serialize() const {
    return {
        {"type", name_}, 
        {"glyph", std::string(1, glyph_)},
        {"transform", transform_.serialize()}
    };
}

void Object::deserialize(const nlohmann::json& json) {
    name_ = json.at("type");
    std::string s = json.at("glyph");
    glyph_ = s[0];
    transform_ = Transform(json.at("transform"));
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

char& Object::glyph() {
    return glyph_;
}

const char& Object::glyph() const {
    return glyph_;
}

bool Object::isEmpty() const noexcept {
    return true;
}

Empty::Empty(World& world, Transform transform, const std::string& name) : Object(world, transform, name) {}

void Empty::registerEmpty() {
    Object::Object_Factory.registerType<Empty>("Empty");
}

bool Empty::isEmpty() const noexcept {
    return true;
}

Wall::Wall(World& world, Transform transform, const std::string& name) : Object(world, transform, name) {}

void Wall::registerWall() {
    Object::Object_Factory.registerType<Wall>("Wall");
}

bool Wall::isEmpty() const noexcept {
    return false;
}