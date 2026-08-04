#pragma once

#include <Factory.hpp>
#include <Collision.hpp>

#include <nlohmann/json.hpp>

#include <cstdint>

class World;

/// @brief This is the positional and rotational
/// structure. Every object within the world has this.
/// Rotation is represented as radians. 
struct Transform {
    Transform();
    Transform(Vector2 vec, double rot);
    Transform(double x, double y, double rot);
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
    Object(World& world, Transform transform = Transform(), const std::string& name = " ", char glyph = ' ');

    virtual ~Object();

    /// @brief Serializes an object's data to JSON (the base does nothing)
    /// @return empty JSON
    virtual nlohmann::json serialize() const;
    
    /// @brief constructs an object from JSON (this does nothing)
    /// @param json 
    virtual void deserialize(const nlohmann::json& json);

    /// @brief Gets this object's current transform
    /// @return this object's current transform (const reference)
    const Transform& transform() const;

    /// @brief Updates the transform to a new location, updates World as well
    /// @param transform 
    void setTransform(const Transform& transform);
    
    /// @brief Updates the position of this object to a new location, updates the world as well
    /// @param position 
    void setPosition(Vector2 position);

    /// @brief Updates the rotation of this object to a new location, updates the world as well 
    /// @param rotation 
    void setRotation(double rotation);

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

    /// @brief Gets the current glyph of this object
    /// @return the current glyph of this object (reference)
    char& glyph();

    /// @brief Gets the current glyph of this object
    /// @return the current glyph of this object (const reference)
    const char& glyph() const;

    /// @brief Gets whether this object is empty or not
    /// @return true if it is empty false if not
    virtual bool isEmpty() const noexcept;

private:
    World* world_;

    Transform transform_;

    std::string name_;
    char glyph_;

    friend class World;
};

/// @brief An empty square, sublass of Object, does nothing
class Empty : public Object {
public:
    Empty(World& world, Transform transform = Transform(), const std::string& name = "Empty");

    static void registerEmpty();

    bool isEmpty() const noexcept override;
};

/// @brief A wall, subclass of Object something to run into
class Wall : public Object {
public:
    Wall(World& world, Transform transform = Transform(), const std::string& name = "Wall");

    static void registerWall();

    bool isEmpty() const noexcept override;
};