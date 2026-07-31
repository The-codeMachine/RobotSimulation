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
            out += map_[convert_to_1D_({x, y})]->toChar();
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
    
    for (size_t i = 0; i < map_.size(); ++i) {
        if (i % ROW_SIZE_ == 0 && i != 0)
            file << std::endl;
        
        Transform t = map_[i]->transform();

        file << t.position.x << "," << t.position.y << "," << t.rotation << "," << static_cast<int>(map_[i]->getType()) << ";";
    }
    
    file.close();
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

void World::construct_from_string_(const std::string& world) {
    if (world.empty()) return;

    struct ParsedObject {
        uint32_t x, y;
        double rotation;
        ObjectType typeChar;
    };
    std::vector<ParsedObject> parsedObjects;
    
    uint32_t max_x = 0;
    uint32_t max_y = 0;

    std::stringstream ss(world);
    std::string objectBlock;

    while (std::getline(ss, objectBlock, ';')) {
        if (objectBlock.find_first_not_of(" \t\n\r") == std::string::npos) {
            continue;
        }

        std::stringstream blockStream(objectBlock);
        std::string xStr, yStr, rotStr, charStr;

        if (std::getline(blockStream, xStr, ',') &&
            std::getline(blockStream, yStr, ',') &&
            std::getline(blockStream, rotStr, ',') &&
            std::getline(blockStream, charStr)) {

            ParsedObject obj;
            obj.x = std::stoul(xStr);
            obj.y = std::stoul(yStr);
            obj.rotation = std::stod(rotStr);
            
            size_t firstChar = charStr.find_first_not_of(" \t\n\r");
            obj.typeChar = static_cast<ObjectType>(charStr[firstChar] - '0');

            if (obj.x > max_x) max_x = obj.x;
            if (obj.y > max_y) max_y = obj.y;

            parsedObjects.push_back(obj);
        }
    }

    ROW_SIZE_ = max_x + 1;
    uint32_t totalRows = max_y + 1;
    map_.resize(ROW_SIZE_ * totalRows);

    /*
    TODO:
    Make the std::make_unique<Object> below change based off the
    type of object we are constructing. 
    */

    for (const auto& obj : parsedObjects) {
        uint32_t index = obj.y * ROW_SIZE_ + obj.x;
        map_[index] = std::make_unique<Object>(*this, Transform({obj.x, obj.y}, obj.rotation), obj.typeChar);
    }
}
