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
/// Object.
///
class World {
public:
    explicit World(const nlohmann::json& world);
    explicit World(const std::filesystem::path& worldFile);

    /// @brief Gets the current object at that position
    /// @return the current object at that position (reference)
    Object& at(Vector2 pos);

    /// @brief Gets the current object at that position
    /// @return the current object at that position (const reference)
    const Object& at(Vector2 pos) const;

    /// @brief Moves an object to a new Transform, checks for collisions
    /// @param obj 
    /// @param newTransform 
    void moveObject(Object& obj, const Transform& newTransform);

    /// @brief changes the ObjectType at a position to the new value
    /// @param pos
    /// @param value 
    void replaceObject(std::unique_ptr<Object> value);

    /// @brief converts the world into string for visual representation
    /// @return the world as a string
    std::string toString() const noexcept;

    /// @brief saves the world's current state to a file
    /// @param path 
    void saveToFile(const std::filesystem::path& path) const;

private:
    /// @brief converts (x, y) to a 1D index
    /// @param x as base-0
    /// @param y as base-0
    /// @return (x, y) as a 1D index
    uint32_t convert_to_1D_(Vector2 vec) const noexcept;

    /// @brief checks whether (x, y) is a valid position
    /// @param x as base-0
    /// @param y as base-0
    /// @return true if it is valid
    bool valid_position_(Vector2 vec) const noexcept;

    /// @brief constructs the world from a string
    /// @param world 
    void deserialize_(const nlohmann::json& world);

    /// @brief converts this world and all its information to JSON
    /// @return this world's JSON representation
    nlohmann::json serialize_() const;

private:
    std::vector<std::unique_ptr<Object>> map_;

    uint32_t ROW_SIZE_;
    uint32_t ROW_AMOUNT_;

private:
    uint32_t WORLD_FILE_VERSION = 1;

};