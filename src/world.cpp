#include <World.hpp>

#include <fstream>
#include <sstream>
#include <stdexcept>

World::World(const std::string& world) {
    m_constructFromString(world);
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

    std::stringstream buffer;
    buffer << file.rdbuf();

    m_constructFromString(buffer.str());
}

ObjectType World::at(int x, int y) const {
    if (!m_validPosition(x, y))
        throw std::runtime_error("(x, y) is an invalid position for this world");

    return m_map[m_convertTo1D(x, y)];
}

ObjectType World::at(Location location) const {
    return at(location.x, location.y);
}

void World::update(int x, int y, ObjectType value) {
    if (!m_validPosition(x, y))
        throw std::runtime_error("(x, y) is an invalid position for this world");

    m_map[m_convertTo1D(x, y)] = value;
}

void World::update(Location location, ObjectType value) {
    if (!m_validPosition(location.x, location.y))
        throw std::runtime_error("(x, y) is an invalid position for this world");

    m_map[m_convertTo1D(location.x, location.y)] = value;
}

ObjectType World::m_convertToObjectType(char c) {
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

uint32_t World::m_convertTo1D(int x, int y) const noexcept {
    return y * ROW_SIZE + x;
}

bool World::m_validPosition(int x, int y) const noexcept {
    uint32_t pos = m_convertTo1D(x, y);
    return pos >= 0 && pos < m_map.size();
}

void World::m_constructFromString(const std::string& world) {
    ROW_SIZE = -1;

    for (uint32_t i = 0; i < world.size(); ++i) {
        const char& c = world[i];

        if (c == '\n' && ROW_SIZE == -1)
            ROW_SIZE = i;
        else {
            m_map[i] = m_convertToObjectType(c);
        }
    }
}