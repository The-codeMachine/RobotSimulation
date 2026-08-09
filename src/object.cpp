#include <Object.hpp>
#include <World.hpp>

Transform::Transform() : position({0, 0}), rotation(0) {}
Transform::Transform(Vector2 vec, double rot) : position(vec), rotation(rot) {}
Transform::Transform(double x, double y, double rot) : position({x, y}), rotation(rot) {}

nlohmann::json Transform::serialize() const {
    nlohmann::json json;

    json["vector2"] = position.serialize();
    json["rotation"] = rotation;

    return json;
}

void Transform::deserialize(const nlohmann::json& json) {
    position.deserialize(json.at("vector2"));
    rotation = json.at("rotation");
}

Object::Object(World& world, Transform transform, const std::string& name, char glyph) 
            : world_(&world), transform_(transform), name_(name), glyph_(glyph) {}

Object::~Object() = default;

nlohmann::json Object::serialize() const {
    nlohmann::json json;
    if (collider_ != nullptr)
        json["collider"] = collider_->serialize();

    json["type"] = name_;
    json["glyph"] = std::string(1, glyph_);
    json["transform"] = transform_.serialize();

    return json;
}

void Object::deserialize(const nlohmann::json& json) {
    name_ = json.at("type");
    std::string s = json.at("glyph");
    glyph_ = s[0];
    transform_.deserialize(json.at("transform"));
    
    if (json.contains("collider")) {
        collider_ = Collider::Collider_Factory.create(json.at("collider").at("type").get<std::string>());
        collider_->deserialize(json.at("collider"));
    }
}

const Transform& Object::transform() const {
    return transform_;
}

void Object::setTransform(const Transform& transform) {
    world_->moveObject(*this, transform);
}

void Object::setPosition(Vector2 position) {
    setTransform(Transform(position, transform().rotation));
}

void Object::setRotation(double rotation) {
    setTransform(Transform(transform().position, rotation));
}

Collider& Object::collider() {
    return *collider_;
}

const Collider& Object::collider() const {
    return *collider_;
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

void Wall::deserialize(const nlohmann::json& json) {
    Object::deserialize(json);
    if (collider_ == nullptr)
        collider_ = Collider::Collider_Factory.create("AABBCollider");
}

bool Wall::isEmpty() const noexcept {
    return false;
}