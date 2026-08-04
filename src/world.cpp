#include <World.hpp>

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>

World::World(const nlohmann::json& world) : ROW_SIZE_(-1) {
    deserialize_(world);
}

World::World(const std::filesystem::path& worldFile) : ROW_SIZE_(-1) {
    if (!std::filesystem::exists(worldFile) 
        || !std::filesystem::is_regular_file(worldFile)) {
            throw std::runtime_error("World file does not exist, or is an invalid file");
    }

    std::ifstream file(worldFile);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file (check permissions)");
    }

    deserialize_(nlohmann::json::parse(file));
}

Object& World::at(Vector2 pos) {
    if (!valid_position_(pos))
        throw std::runtime_error("Invalid position");

    return *map_[convert_to_1D_(pos)];
}

const Object& World::at(Vector2 pos) const {
    if (!valid_position_(pos))
        throw std::runtime_error("Invalid position");

    return *map_[convert_to_1D_(pos)];
}

void World::moveObject(Object& obj, const Transform& newTransform) {
    Vector2 oldPosition = obj.transform().position;
    Vector2 newPosition = newTransform.position;

    if (!valid_position_(newPosition))
        throw std::runtime_error("(x, y) is an invalid position for this world");

    uint32_t oldIndex = convert_to_1D_(oldPosition);
    uint32_t newIndex = convert_to_1D_(newPosition);
    
    std::cout << "old index: " << oldIndex << "\n";
    std::cout << "new index: " << newIndex << "\n";

    if (&*map_[oldIndex] != &obj)
        throw std::runtime_error("Object is not located at its recorded position");

    if (oldIndex == newIndex) {
        obj.transform_ = newTransform;
        return;
    }

    std::cout << oldPosition.x << ", " << oldPosition.y << "\n";
    std::cout << newPosition.x << ", " << newPosition.y << "\n";

    if (!map_[newIndex]->isEmpty())
        throw std::runtime_error("Cannot move object into an occupied position");

    map_[newIndex] = std::move(map_[oldIndex]);
    map_[newIndex]->transform_ = newTransform;
    map_[oldIndex] = std::move(Object::Object_Factory.create("Empty", *this, Transform(oldPosition, 0)));
}

std::optional<CollisionResult> World::cast(const Trajectory& trajectory, const Object& ignore) const {
    std::optional<CollisionResult> closest;

    for (const auto& object : map_) {
        if (object.get() == &ignore)
            continue;

        if (object->isEmpty())
            continue;

        // Currently every non-empty object is treated
        // as a 1x1 wall-like AABB.
        const Vector2 position = object->transform().position;
        
        AABBCollider collider(position, {position.x + 1.0, position.y + 1.0});
        auto collision = castTrajectory(trajectory, collider, object.get());

        if (!collision)
            continue;

        if (!closest || collision->time < closest->time) {
            closest = std::move(collision);
        }
    }

    return closest;
}

void World::replaceObject(std::unique_ptr<Object> value) {
    Vector2 pos = value->transform().position;
    if (!valid_position_(pos))
        throw std::runtime_error("(x, y) is an invalid position for this world");

    map_[convert_to_1D_(pos)] = std::move(value);
}

std::string World::toString() const noexcept {
    std::string out;

    for (double y = 0; y < ROW_AMOUNT_; ++y) {
        for (double x = 0; x < ROW_SIZE_; ++x) {
            out += map_[convert_to_1D_({x, y})]->glyph();
        }

        if (y + 1 < ROW_AMOUNT_)
            out += '\n';
    }

    return out;
}

void World::saveToFile(const std::filesystem::path& path) const {
    std::ofstream file(path);

    if (!file)
        throw std::runtime_error("Could not open file at: " + path.string());
    
    file << serialize_();
    
    file.close();
}

uint32_t World::convert_to_1D_(Vector2 vec) const noexcept {
    return std::round(vec.y) * ROW_SIZE_ + std::round(vec.x);
}

bool World::valid_position_(Vector2 vec) const noexcept {
    return vec.x < ROW_SIZE_ && vec.y < ROW_AMOUNT_;
}

void World::deserialize_(const nlohmann::json& world) {
    if (world.at("version") != WORLD_FILE_VERSION)
        throw std::runtime_error("Invalid world file version");

    ROW_SIZE_ = world.at("ROW_SIZE");
    ROW_AMOUNT_ = world.at("ROW_AMOUNT");
    map_.reserve(ROW_SIZE_ * ROW_AMOUNT_);

    // set all objects in the world to empty 
    for (size_t i = 0; i < ROW_AMOUNT_ * ROW_SIZE_; ++i) {
        Transform t(i % ROW_SIZE_, i / ROW_SIZE_, 0);
        map_.push_back(Object::Object_Factory.create("Empty", *this, t));
    }

    for (const auto j : world.at("objects")) {
        std::string name = j.at("type");
        Transform t(j.at("transform"));
        
        if (!valid_position_(t.position))
            throw std::runtime_error("Invalid object position according to Row size and amount");
        
        std::unique_ptr<Object> obj = Object::Object_Factory.create(name, *this, t);
        if (obj == nullptr)
            throw std::runtime_error("Failed to construct map, no conversion for: " + name + ". Check that you registered types before construction");

        replaceObject(std::move(obj));
        map_[convert_to_1D_(t.position)]->deserialize(j);
    }
}

nlohmann::json World::serialize_() const {
    nlohmann::json json;

    json["version"] = WORLD_FILE_VERSION;
    json["ROW_SIZE"] = ROW_SIZE_;
    json["ROW_AMOUNT"] = map_.size() / ROW_SIZE_;

    nlohmann::json objects;
    for (const auto& o : map_) {
        // skip empty objects
        if (o->name() == "Empty")
            continue;

        objects.push_back(o->serialize());
    }

    json["objects"] = objects;

    return json;
}