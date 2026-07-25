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
    Location(uint32_t x, uint32_t y) : x(x), y(y) {}
    
    uint32_t x;
    uint32_t y;
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
    explicit World(const std::string& world);
    explicit World(const std::filesystem::path& worldFile);

    /// @brief finds the ObjectType at (x, y) and returns it
    /// @param x as base-0
    /// @param y as base-0
    /// @return the ObjectType located at (x, y) 
    ObjectType at(uint32_t x, uint32_t y) const;

    /// @brief finds the ObjectType at location and returns it
    /// @param location 
    /// @return the ObjectType located at location
    ObjectType at(Location location) const;

    /// @brief changes the ObjectType at (x, y) to the new value
    /// @param x as base-0
    /// @param y as base-0
    /// @param value 
    void update(uint32_t x, uint32_t y, ObjectType value);

    /// @brief changes the ObjectType at location to the new value
    /// @param x as base-0
    /// @param y as base-0
    /// @param value 
    void update(Location location, ObjectType value);

    /// @brief converts the world into string for visual representation
    /// @return the world as a string
    std::string toString() const noexcept;

private:
    /// @brief converts c to an ObjectType if possible
    /// @param c 
    /// @return c as an ObjectType
    static ObjectType convert_to_objecttype_(char c);

    /// @brief converts the ObjectType into a character
    /// @param obj 
    /// @return the ObjectType as a character
    static char convert_to_char_(ObjectType obj);

    /// @brief converts (x, y) to a 1D index
    /// @param x as base-0
    /// @param y as base-0
    /// @return (x, y) as a 1D index
    uint32_t convert_to_1D_(uint32_t x, uint32_t y) const noexcept;

    /// @brief checks whether (x, y) is a valid position
    /// @param x as base-0
    /// @param y as base-0
    /// @return true if it is valid
    bool valid_position_(uint32_t x, uint32_t y) const noexcept;

    /// @brief constructs the world from a string
    /// @param world 
    void construct_from_string_(const std::string& world);

private:
    std::vector<ObjectType> map_;

    uint32_t ROW_SIZE_;

};