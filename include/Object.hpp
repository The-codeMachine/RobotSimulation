#pragma once

#include <cstdint>

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
    Vector2 position;
    double rotation;
};

/// @brief 
///
/// Represents an object. Specifies which
/// type of object it is. 
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
static inline constexpr const char* OBJECT_REPRESENTATION = " #R";

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
    Object();
    Object(Transform transform, ObjectType type);

    /// @brief Gets the Object's current Transform
    /// @return the Object's current Transform
    Transform& getTransform();

    /// @brief Gets the Object's current Transform
    /// @return the Object's current Transform
    Transform getTransform() const noexcept;

    /// @brief Get's which type of object this is
    /// @return which type of object this is
    ObjectType getType() const noexcept;

    /// @brief Converts this object into a representable char (for the world)
    /// @return this object as a char
    char toChar() const noexcept;

private:
    Transform transform_;
    ObjectType type_;

};