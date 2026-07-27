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

Object World::at(Vector2 pos) const {
    return map_[convert_to_1D_(pos.x, pos.y)];
}

void World::update(Object value) {
    Vector2 pos = value.getTransform().position;
    if (!valid_position_(pos.x, pos.y))
        throw std::runtime_error("(x, y) is an invalid position for this world");

    map_[convert_to_1D_(pos.x, pos.y)] = value;
}

std::string World::toString() const noexcept {
    std::string out;

    const uint32_t rows = map_.size() / ROW_SIZE_;

    for (uint32_t y = 0; y < rows; ++y) {
        for (uint32_t x = 0; x < ROW_SIZE_; ++x) {
            out += map_[convert_to_1D_(x, y)].toChar();
        }

        if (y + 1 < rows)
            out += '\n';
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

uint32_t World::convert_to_1D_(uint32_t x, uint32_t y) const noexcept {
    return y * ROW_SIZE_ + x;
}

bool World::valid_position_(uint32_t x, uint32_t y) const noexcept {
    uint32_t pos = convert_to_1D_(x, y);
    return pos >= 0 && pos < map_.size();
}

void World::construct_from_string_(const std::string& world) {
    map_.resize(world.size());

    std::stringstream ss(world);
    std::string row;
    uint32_t mapIndex = 0;

    while (std::getline(ss, row)) {
        if (ROW_SIZE_ == -1) {
            ROW_SIZE_ = row.size();
        } else if (row.size() != static_cast<size_t>(ROW_SIZE_)) {
            throw std::runtime_error("Invalid world string: irregular row size detected.");
        }

        for (char c : row) {
            map_[mapIndex++] = Object({mapIndex % ROW_SIZE_, mapIndex / ROW_SIZE_}, convert_to_objecttype_(c));
        }
    }
}