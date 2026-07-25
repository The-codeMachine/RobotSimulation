#include <World.hpp>

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>

World::World(const std::string& world) : ROW_SIZE_(-1) {
    construct_from_string_(world);
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

    std::stringstream buffer;
    buffer << file.rdbuf();

    construct_from_string_(buffer.str());
}

ObjectType World::at(uint32_t x, uint32_t y) const {
    if (!valid_position_(x, y))
        throw std::runtime_error("(x, y) is an invalid position for this world");

    return map_[convert_to_1D_(x, y)];
}

ObjectType World::at(Location location) const {
    return at(location.x, location.y);
}

void World::update(uint32_t x, uint32_t y, ObjectType value) {
    if (!valid_position_(x, y))
        throw std::runtime_error("(x, y) is an invalid position for this world");

    map_[convert_to_1D_(x, y)] = value;
}

void World::update(Location location, ObjectType value) {
    if (!valid_position_(location.x, location.y))
        throw std::runtime_error("(x, y) is an invalid position for this world");

    map_[convert_to_1D_(location.x, location.y)] = value;
}

std::string World::toString() const noexcept {
    std::string out;
    
    for (uint32_t i = 0; i < map_.size(); i += ROW_SIZE_) {
        for (uint32_t j = 0; j < ROW_SIZE_; ++j) {
            out += convert_to_char_(map_[convert_to_1D_(j, i)]);
        }
    }

    return out;
}

ObjectType World::convert_to_objecttype_(char c) {
    switch (c) {
        case ' ':
            return ObjectType::EMPTY;
        case '#':
            return ObjectType::WALL;
        case 'R':
            return ObjectType::ROBOT;

        default:
            throw std::runtime_error("Invalid ObjectType conversion to char (character is invalid)");
    }
}

char World::convert_to_char_(ObjectType obj) {
    return OBJECT_REPRESENTATION[static_cast<uint8_t>(obj)];
}

uint32_t World::convert_to_1D_(uint32_t x, uint32_t y) const noexcept {
    return y * ROW_SIZE_ + x;
}

bool World::valid_position_(uint32_t x, uint32_t y) const noexcept {
    uint32_t pos = convert_to_1D_(x, y);
    return pos >= 0 && pos < map_.size();
}

void World::construct_from_string_(const std::string& world) {
    map_.resize(world.size()); // Note: Might allocate slightly more than needed due to ignored newlines

    std::stringstream ss(world);
    std::string row;
    uint32_t mapIndex = 0;

    while (std::getline(ss, row)) {
        // Print the row with a newline to match original std::cout behavior
        std::cout << row << "\n";

        // Validate row size
        if (ROW_SIZE_ == -1) {
            ROW_SIZE_ = row.size();
        } else if (row.size() != static_cast<size_t>(ROW_SIZE_)) {
            throw std::runtime_error("Invalid world string: irregular row size detected.");
        }

        // Populate the map
        for (char c : row) {
            map_[mapIndex++] = convert_to_objecttype_(c);
        }
    }
}