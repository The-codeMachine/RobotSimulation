#include <World.hpp>

#include <fstream>
#include <sstream>
#include <stdexcept>

World::World(const nlohmann::json& world) {
    deserialize_(world);
    sink_ = std::make_unique<DebugChangeSink>();
}

World::World(const std::filesystem::path& worldFile) {
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
    return *map_.at(pos);
}

const Object& World::at(Vector2 pos) const {
    return *map_.at(pos);
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
    Transform oldTransform = obj.transform();
    Vector2 oldPosition = oldTransform.position;
    Vector2 newPosition = newTransform.position;

    std::cout << oldPosition.toString() << "\n";
    std::cout << newPosition.toString() << "\n";

    auto oldIt = map_.find(oldPosition);
    if (oldIt == map_.end() || oldIt->second.get() != &obj)
        throw std::runtime_error("Object is not located at its recorded position");

    if (oldPosition == newPosition) {
        obj.transform_ = newTransform;
        emit("object.transform", {
            {"object", obj.id()},
            {"old", oldTransform.serialize()},
            {"new", newTransform.serialize()},
        });
        return;
    }

    // checks whether or not the new position is occupied
    auto newIt = map_.find(newPosition);
    if (newIt != map_.end() && !newIt->second->isEmpty())
        throw std::out_of_range(
            "Cannot move object: target position " + newPosition.toString() +
            " is already occupied");

    auto moved = std::move(oldIt->second);
    Object* movedPtr = moved.get();
    movedPtr->transform_ = newTransform;
    map_[newPosition] = std::move(moved);
    map_[oldPosition] = Object::Object_Factory.create("Empty", *this, Transform(oldPosition, 0));

    emit("object.transform", {
        {"object", movedPtr->id()},
        {"old", oldTransform.serialize()},
        {"new", newTransform.serialize()},
    });
}

std::optional<CollisionResult> World::cast(const Trajectory& trajectory, const Object& ignore) const {
    std::optional<CollisionResult> closest;

    for (const auto& [pos, object] : map_) {
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

    for (const auto& [pos, obj] : map_) {
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

    map_[pos] = std::move(value);
}

void World::saveToFile(const std::filesystem::path& path) const {
    std::ofstream file(path);

    if (!file)
        throw std::runtime_error("Could not open file at: " + path.string());
    
    file << serialize_();
    
    file.close();
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

    const auto objectsIt = world.find("objects");
    if (objectsIt == world.end() || !objectsIt->is_array())
        throw std::runtime_error("World file is missing an 'objects' array");

    std::unordered_map<Vector2, std::unique_ptr<Object>> newMap;

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
        Vector2 pos = t.position;

        auto obj = Object::Object_Factory.create(name, *this, t);

        if (!obj)
            throw std::runtime_error(
                "Failed to construct object of type '" +
                name +
                "'. Check that the type was registered before loading"
            );

        obj->deserialize(j);

        newMap[pos] = std::move(obj);
    }

    map_ = std::move(newMap);
}

nlohmann::json World::serialize_() const {
    nlohmann::json json;

    json["version"] = WORLD_FILE_VERSION;

    nlohmann::json objects;
    for (const auto& [pos, o] : map_) {
        // skip empty objects
        if (o->name() == "Empty")
            continue;

        objects.push_back(o->serialize());
    }

    json["objects"] = objects;

    return json;
}