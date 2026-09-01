#pragma once

#include <nlohmann/json.hpp>

/// @brief This is one position in the world. It 
/// consists of an x and y value. They can be
/// any real value that the world will support. 
struct Vector2 { 
    double x;
    double y;

    /// @brief Serializes the Vector2 into JSON
    /// @return the Vector2 as JSON
    nlohmann::json serialize() const;

    /// @brief Deserializes the Vector2 from JSON
    /// @param json 
    void deserialize(const nlohmann::json& json);

    bool operator==(Vector2 other) const noexcept;
    bool operator!=(Vector2 other) const noexcept;

    Vector2 operator+(Vector2 other) const noexcept;
    Vector2 operator-(Vector2 other) const noexcept;
    Vector2 operator*(double scalar) const noexcept;
    Vector2 operator/(double scalar) const noexcept;
    Vector2 operator*(Vector2 other) const noexcept;
    Vector2 operator/(Vector2 other) const noexcept;

    void operator+=(Vector2 other);
    void operator-=(Vector2 other);
    void operator*=(double scalar);
    void operator/=(double scalar);
    void operator*=(Vector2 other);
    void operator/=(Vector2 other);

    static double dot(Vector2 lhs, Vector2 rhs) noexcept;
    static double lengthSquared(Vector2 value) noexcept;
    static double length(Vector2 value) noexcept;

    static Vector2 normalize(Vector2 value) noexcept;
    static Vector2 clamp(Vector2 value, Vector2 minimum, Vector2 maximum) noexcept;

    /// @brief Converts a Vector2 into a string
    /// @return a string representing a Vector2
    std::string toString() const noexcept;
};
