#include <World.hpp>

#include <fstream>
#include <sstream>
#include <stdexcept>

World::World(const nlohmann::json& world) : ROW_SIZE_(-1) {
    deserialize_(world);
    sink_ = std::make_unique<DebugChangeSink>();
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
    sink_ = std::make_unique<DebugChangeSink>();
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

ChangeSink& World::sink() {
    return *sink_;
}

const ChangeSink& World::sink() const {
    return *sink_;
}

void World::setSink(std::unique_ptr<ChangeSink> sink) {
    sink_ = std::move(sink);
}

void World::emit(const ChangeEvent& event) const {
    if (!sink_)
        throw std::runtime_error("ChangeSink has not been set yet");
    
    sink_->publish(event);
}

void World::emit(const std::string& type, const nlohmann::json& data) {
    emit(ChangeEvent{sequence_++, type, data});
}

void World::moveObject(Object& obj, const Transform& newTransform) {
    Vector2 oldPosition = obj.transform().position;
    Vector2 newPosition = newTransform.position;

    if (!valid_position_(newPosition))
        throw std::runtime_error("(x, y) is an invalid position for this world");

    size_t oldIndex = convert_to_1D_(oldPosition);
    size_t newIndex = convert_to_1D_(newPosition);

    if (&*map_[oldIndex] != &obj)
        throw std::runtime_error("Object is not located at its recorded position");

    if (oldIndex == newIndex) {
        obj.transform_ = newTransform;
        return;
    }

    if (!map_[newIndex]->isEmpty())
        throw std::runtime_error("Cannot move object into an occupied position");

    map_[newIndex] = std::move(map_[oldIndex]);
    map_[newIndex]->transform_ = newTransform;
    map_[oldIndex] = std::move(Object::Object_Factory.create("Empty", *this, Transform(oldPosition, 0)));

    emit("object.transform", {
        {"object", map_[newIndex]->id()},
        {"old", map_[newIndex]->transform().serialize()}, 
        {"new", newTransform.serialize()},
    });
}

std::optional<CollisionResult> World::cast(const Trajectory& trajectory, const Object& ignore) const {
    std::optional<CollisionResult> closest;

    for (const auto& object : map_) {
        if (object.get() == &ignore)
            continue;

        if (object->isEmpty())
            continue;

        const Vector2 position = object->transform().position;
        Collider& collider = object->collider();
        auto collision = castTrajectory(trajectory, collider, object.get());

        if (!collision)
            continue;

        if (!closest || collision->time < closest->time) {
            closest = std::move(collision);
        }
    }

    return closest;
}

std::vector<Detection> World::sense(const SensorShape& shape) {
    std::vector<Detection> out;

    for (const auto& obj : map_) {
        const auto& transform = obj->transform();        

        if (obj->name() != "Empty" && shape.contains(transform)) {
            // copies the object and its position
            out.push_back({std::move(clone_object_(*obj)), transform.position});
        }
    }

    return out;
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

size_t World::convert_to_1D_(Vector2 vec) const noexcept {
    return std::round(vec.y) * ROW_SIZE_ + std::round(vec.x);
}

bool World::valid_position_(Vector2 vec) const noexcept {
    return vec.x < ROW_SIZE_ && vec.y < ROW_AMOUNT_;
}

std::unique_ptr<Object> World::clone_object_(const Object& original) {
    auto copy = Object::Object_Factory.create(original.name(), *this, original.transform());
    if (!copy)
        throw std::runtime_error("Failed to create a copy of the original object");
    
    copy->deserialize(original.serialize());
    return std::move(copy);
}

void World::deserialize_(const nlohmann::json& world) {
    if (!world.is_object())
        throw std::runtime_error("World file root must be a JSON object");

    const auto versionIt = world.find("version");
    if (versionIt == world.end() || !versionIt->is_number_integer())
        throw std::runtime_error("World file is missing a valid integer 'version'");

    if (versionIt->get<int>() != WORLD_FILE_VERSION)
        throw std::runtime_error("Invalid world file version");

    const auto rowSizeIt = world.find("ROW_SIZE");
    if (rowSizeIt == world.end() || !rowSizeIt->is_number_unsigned())
        throw std::runtime_error("World file is missing a valid 'ROW_SIZE'");

    const auto rowAmountIt = world.find("ROW_AMOUNT");
    if (rowAmountIt == world.end() || !rowAmountIt->is_number_unsigned())
        throw std::runtime_error("World file is missing a valid 'ROW_AMOUNT'");

    ROW_SIZE_ = rowSizeIt->get<size_t>();
    ROW_AMOUNT_ = rowAmountIt->get<size_t>();

    if (ROW_SIZE_ == 0)
        throw std::runtime_error("World ROW_SIZE must be greater than zero");

    if (ROW_AMOUNT_ == 0)
        throw std::runtime_error("World ROW_AMOUNT must be greater than zero");

    if (ROW_AMOUNT_ > std::numeric_limits<size_t>::max() / ROW_SIZE_)
        throw std::runtime_error("World dimensions are too large");

    const size_t mapSize = ROW_SIZE_ * ROW_AMOUNT_;

    const auto objectsIt = world.find("objects");
    if (objectsIt == world.end() || !objectsIt->is_array())
        throw std::runtime_error("World file is missing an 'objects' array");

    std::vector<std::unique_ptr<Object>> newMap;
    newMap.reserve(mapSize);

    for (size_t i = 0; i < mapSize; ++i) {
        Transform t(
            static_cast<double>(i % ROW_SIZE_),
            static_cast<double>(i / ROW_SIZE_),
            0
        );

        auto obj = Object::Object_Factory.create("Empty", *this, t);

        if (!obj)
            throw std::runtime_error("Failed to create default Empty object");

        newMap.push_back(std::move(obj));
    }

    // keeps track of the positions that have already been populated
    std::vector<bool> occupied(mapSize, false);

    for (const auto& j : *objectsIt) {
        if (!j.is_object())
            throw std::runtime_error("Every entry in 'objects' must be a JSON object");

        const auto typeIt = j.find("type");
        if (typeIt == j.end() || !typeIt->is_string())
            throw std::runtime_error("World object is missing a valid string 'type'");

        const auto transformIt = j.find("transform");
        if (transformIt == j.end() || !transformIt->is_object())
            throw std::runtime_error("World object is missing a valid 'transform'");

        const std::string name = typeIt->get<std::string>();

        Transform t;
        t.deserialize(*transformIt);

        if (!valid_position_(t.position))
            throw std::runtime_error("Invalid object position according to world dimensions");

        const size_t index = convert_to_1D_(t.position);

        if (index >= mapSize)
            throw std::runtime_error("Object position converted to an invalid map index");

        if (occupied[index])
            throw std::runtime_error("Multiple objects occupy the same world position");

        auto obj = Object::Object_Factory.create(name, *this, t);

        if (!obj)
            throw std::runtime_error(
                "Failed to construct object of type '" +
                name +
                "'. Check that the type was registered before loading"
            );

        obj->deserialize(j);

        newMap[index] = std::move(obj);
        occupied[index] = true;
    }

    map_ = std::move(newMap);
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