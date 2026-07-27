#pragma once

#include <Object.hpp>

#include <string>
#include <filesystem>

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

    /// @brief finds the ObjectType at a specific position and returns it
    /// @param pos 
    /// @return the ObjectType located at a specific position
    Object at(Vector2 pos) const;

    /// @brief changes the ObjectType at a position to the new value
    /// @param pos
    /// @param value 
    void update(Object value);

    /// @brief converts the world into string for visual representation
    /// @return the world as a string
    std::string toString() const noexcept;

private:
    /// @brief converts c to an ObjectType if possible
    /// @param c 
    /// @return c as an ObjectType
    static ObjectType convert_to_objecttype_(char c);

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
    std::vector<Object> map_;

    uint32_t ROW_SIZE_;

};