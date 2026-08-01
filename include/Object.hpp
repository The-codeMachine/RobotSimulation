#pragma once

#include "Factory.hpp"

#include <nlohmann/json.hpp>

#include <cstdint>

class World;

/// @brief This is one position in the world. It 
/// consists of an x and y value. They can be
/// any real value that the world will support. 
struct Vector2 {
    uint32_t x;
    uint32_t y;
};

/// @brief This is the positional and rotational
/// structure. Every object within the world has this.
/// Rotation is represented as radians. 
struct Transform {
    Transform();
    Transform(Vector2 vec, double rot);
    Transform(uint32_t x, uint32_t y, double rot);
    Transform(const nlohmann::json& json);

    nlohmann::json serialize() const;

    Vector2 position;
    double rotation;
};

/// @brief 
/// This represents an object in the world. It holds 
/// position, rotational, and graphical information. 
/// It allows you to update, and get all of the 
/// variables it encapsulates. 
///
/// Maybe find a way to represent the rotation of the
/// object but we might need something more than text
/// to do that, and that is more of an issue for the
/// world. 
class Object {
public:
    static inline Factory::Factory<Object, World&, Transform> Object_Factory;

public:
    Object();
    Object(World& world, Transform transform = Transform(), const std::string& name = "");

    virtual ~Object();

    /// @brief Serializes an object's data to JSON (the base does nothing)
    /// @return empty JSON
    virtual nlohmann::json serialize() const;
    
    /// @brief constructs an object from JSON (this does nothing)
    /// @param json 
    virtual void deserialize(const nlohmann::json& json);

    /// @brief Gets this object's current transform
    /// @return this object's current transform (reference)
    Transform& transform();

    /// @brief Gets this object's current transform
    /// @return this object's current transform (const reference)
    const Transform& transform() const;

    /// @brief Gets the current world this object is in
    /// @return the current world this object is it (reference)
    World& world();

    /// @brief Gets the current world this object is in
    /// @return the current world this object is it (const reference)
    const World& world() const;

    /// @brief Gets the current name of this object
    /// @return the current name of this object (reference)
    std::string& name();

    /// @brief Gets the current name of this object
    /// @return the current name of this object (const reference)
    const std::string& name() const;

private:
    World* world_;

    Transform transform_;
    std::string name_;

};
