#pragma once

#include <cstdint>
#include <string>
#include <filesystem>

/// @brief 
///
/// Represents an object. Specifies which
/// type of object it is. There might be
/// an object class later down the line
/// once we consolidate what a specific
/// object is. Once that is created,
/// this should be moved there. 
///
enum class ObjectType : uint8_t {
    EMPTY = 0,
    WALL,
    ROBOT
};

/*
This correlates to the ObjectType. At the
index of the ObjectType, it's object 
representation as a character. We recommend
a padding of a space (' ').
*/
static inline constexpr const char* OBJECT_REPRESENTATION = ".#R";

/// @brief 
///
/// Location represents a single position
/// within the world. This consists of an 
/// integer (x, y) position. Currently,
/// this is 2D only. The world can expand 
/// infinitely from positive x/y to negative
/// x/y (to the 64-integer limit).
///
struct Location {
    Location(int64_t x, int64_t y) : x(x), y(y) {}
    
    int64_t x;
    int64_t y;
};

/// @brief 
///
/// This is the World class. It encapsulates
/// all objects within a world. This includes:
/// - The Robot
/// - Walls
/// - Interactable objects
/// - Custom objects
///
/// It allows you to access which object are at
/// which spot. Eventually, this will own the
/// Object, but currently that class does not 
/// exist. 
///
class World {
public:
    World(const std::string& world);
    World(const std::filesystem::path& worldFile);

    /// @brief finds the ObjectType at (x, y) and returns it
    /// @param x as base-0
    /// @param y as base-0
    /// @return the ObjectType located at (x, y) 
    ObjectType at(int64_t x, int64_t y) const noexcept;

    /// @brief finds the ObjectType at location and returns it
    /// @param location 
    /// @return the ObjectType located at location
    ObjectType at(Location location) const noexcept;

    /// @brief changes the ObjectType at (x, y) to the new value
    /// @param x as base-0
    /// @param y as base-0
    /// @param value 
    void update(int64_t x, int64_t y, ObjectType value);

    /// @brief changes the ObjectType at location to the new value
    /// @param x as base-0
    /// @param y as base-0
    /// @param value 
    void update(Location location, ObjectType value);

private:
    /// @brief converts c to an ObjectType if possible
    /// @param c 
    /// @return c as an ObjectType
    static ObjectType m_convertToObjectType(char c);

    /// @brief converts (x, y) to a 1D index
    /// @param x as base-0
    /// @param y as base-0
    /// @return (x, y) as a 1D index
    uint64_t m_convertTo1D(int64_t x, int64_t y) const noexcept;

    /// @brief checks whether (x, y) is a valid position
    /// @param x as base-0
    /// @param y as base-0
    /// @return true if it is valid
    bool m_validPosition(int64_t x, int64_t y) const noexcept;

    /// @brief constructs the world from a string
    /// @param world 
    void m_constructFromString(const std::string& world);

private:
    std::vector<ObjectType> m_map;

    uint32_t ROW_SIZE;

};