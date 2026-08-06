#include <World.hpp>

#include <fstream>
#include <sstream>
#include <stdexcept>

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

    const size_t rowSize = rowSizeIt->get<size_t>();
    const size_t rowAmount = rowAmountIt->get<size_t>();

    if (rowSize == 0)
        throw std::runtime_error("World ROW_SIZE must be greater than zero");

    if (rowAmount == 0)
        throw std::runtime_error("World ROW_AMOUNT must be greater than zero");

    if (rowAmount > std::numeric_limits<size_t>::max() / rowSize)
        throw std::runtime_error("World dimensions are too large");

    const size_t mapSize = rowSize * rowAmount;

    const auto objectsIt = world.find("objects");
    if (objectsIt == world.end() || !objectsIt->is_array())
        throw std::runtime_error("World file is missing an 'objects' array");

    std::vector<std::unique_ptr<Object>> newMap;
    newMap.reserve(mapSize);

    for (size_t i = 0; i < mapSize; ++i) {
        Transform t(
            static_cast<double>(i % rowSize),
            static_cast<double>(i / rowSize),
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

    ROW_SIZE_ = rowSize;
    ROW_AMOUNT_ = rowAmount;
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