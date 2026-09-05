#pragma once

#include <Object.hpp>
#include <Ray.hpp>
#include <Collision.hpp>
#include <ChangeSink.hpp>
#include <Devices/Sensors/SensorShape.hpp>
#include <Devices/Sensors/Image.hpp>

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
/// World has a ChangeSink, and emit function. To 
/// emit the changes to a specific object, e.g.
/// Robot, Wall, call the emit function with a 
/// specific event you want to be emitted. 
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

    /// @brief Gets the current ChangeSink from World. Might not be a Change
    /// sink, could be a subclass
    /// @return the sink from World (reference)
    ChangeSink& sink();

    /// @brief Gets the current ChangeSink from World. Might not be a Change
    /// sink, could be a subclass
    /// @return the sink from World (const reference)
    const ChangeSink& sink() const;

    /// @brief Sets a new ChangeSink for the World. 
    /// @param sink 
    void setSink(std::unique_ptr<ChangeSink> sink);

    /// @brief Emits a new ChangeEvent to the ChangeSink set to the World
    /// @param event 
    void emit(const ChangeEvent& event) const;

    /// @brief Emits a new ChanngeEvent to the sink set in World. Constructs the ChangeEvent from 
    /// the World's sequence, and the specified type and data.
    /// @param type 
    /// @param data 
    void emit(const std::string& type, const nlohmann::json& data);

    /// @brief Moves an object to a new Transform, checks for collisions
    /// @param obj 
    /// @param newTransform 
    void moveObject(Object& obj, const Transform& newTransform);

    /// @brief Casts a collision ray based off trajectory current treats everything like an AABB wall
    /// @param trajectory 
    /// @param ignore 
    /// @return the result of the collision ray cast
    std::optional<CollisionResult> cast(const Trajectory& trajectory, const Object& ignore) const;

    /// @brief Casts a ray across the world (not implemented)
    /// @return the raycast's result from its calculation 
    std::optional<RaycastResult> raycast() const;

    /// @brief Senses all detections from a shape (needs to be optimized)
    /// @param shape 
    /// @return a vector of detections sensed from the shape
    std::vector<Detection> sense(const SensorShape& shape);

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
    size_t convert_to_1D_(Vector2 vec) const noexcept;

    /// @brief checks whether (x, y) is a valid position
    /// @param x as base-0
    /// @param y as base-0
    /// @return true if it is valid
    bool valid_position_(Vector2 vec) const noexcept;

    /// @brief Clones an object completely, and returns the clone
    /// @param original 
    /// @return the clone from the object
    std::unique_ptr<Object> clone_object_(const Object& original);

    /// @brief constructs the world from a string
    /// @param world 
    void deserialize_(const nlohmann::json& world);

    /// @brief converts this world and all its information to JSON
    /// @return this world's JSON representation
    nlohmann::json serialize_() const;

private:
    std::vector<std::unique_ptr<Object>> map_;

    std::unique_ptr<ChangeSink> sink_;
    uint64_t sequence_ = 0;

    uint32_t ROW_SIZE_;
    uint32_t ROW_AMOUNT_;

private:
    uint32_t WORLD_FILE_VERSION = 1;

};