#include <World.hpp>

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>

World::World(const nlohmann::json& world) : ROW_SIZE_(-1) {
    construct_from_json_(world);
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

    construct_from_json_(nlohmann::json::parse(file));
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

void World::update(std::unique_ptr<Object> value) {
    Vector2 pos = value->transform().position;
    if (!valid_position_(pos))
        throw std::runtime_error("(x, y) is an invalid position for this world");

    map_[convert_to_1D_(pos)] = std::move(value);
}

std::string World::toString() const noexcept {
    std::string out;

    const uint32_t rows = map_.size() / ROW_SIZE_;

    for (uint32_t y = 0; y < rows; ++y) {
        for (uint32_t x = 0; x < ROW_SIZE_; ++x) {
            out += map_[convert_to_1D_({x, y})]->name();
        }

        if (y + 1 < rows)
            out += '\n';
    }

    return out;
}

void World::saveToFile(const std::filesystem::path& path) const {
    std::ofstream file(path);

    if (!file)
        throw std::runtime_error("Could not open file at: " + path.string());
    
    file << serialize();
    
    file.close();
}

uint32_t World::convert_to_1D_(Vector2 vec) const noexcept {
    return vec.y * ROW_SIZE_ + vec.x;
}

bool World::valid_position_(Vector2 vec) const noexcept {
    if (vec.x >= ROW_SIZE_)
        return false;

    uint32_t rows = map_.size() / ROW_SIZE_;
    if (vec.y >= rows)
        return false;

    return true;
}

void World::construct_from_json_(const nlohmann::json& world) {
    if (world.at("version") != WORLD_FILE_VERSION)
        throw std::runtime_error("Invalid world file version");

    ROW_SIZE_ = world.at("ROW_SIZE");

    size_t i = 0;
    for (const auto j : world.at("objects")) {
        std::string name = j.at("type");
        Transform t(j.at("transform"));

        map_.push_back(Object::Object_Factory.create(name, *this, t));
        map_[i]->deserialize(j);
        i++;
    }
}

nlohmann::json World::serialize() const {
    nlohmann::json json;

    json["version"] = WORLD_FILE_VERSION;
    json["ROW_SIZE"] = ROW_SIZE_;

    nlohmann::json objects;
    for (const auto& o : map_) {
        objects.push_back(o->serialize());
    }

    json["objects"] = objects;

    return json;
}