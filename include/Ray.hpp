#pragma once

#include <Collision.hpp>

class Object;

/// @brief A ray originating at a point and extending in a direction.
struct Ray {
    Vector2 origin;
    Vector2 direction;
};

/// @brief The result of intersecting a ray with an object.
struct RaycastResult {
    Object* object;
    Vector2 position;
    Vector2 normal;
    double distance;
};