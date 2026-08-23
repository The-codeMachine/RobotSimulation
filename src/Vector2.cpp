#include <Vector2.hpp>

nlohmann::json Vector2::serialize() const {
    return {
        {"x", x},
        {"y", y}
    };
}

void Vector2::deserialize(const nlohmann::json& json) {
    x = json.at("x");
    y = json.at("y");
}

bool Vector2::operator==(Vector2 other) const noexcept {
    return x == other.x && y == other.y;
}

bool Vector2::operator!=(Vector2 other) const noexcept {
    return !(*this == other);
}

Vector2 Vector2::operator+(Vector2 other) const noexcept {
    return {x + other.x, y + other.y};
}

Vector2 Vector2::operator-(Vector2 other) const noexcept {
    return {x - other.x, y - other.y};
}

Vector2 Vector2::operator*(double scalar) const noexcept {
    return {x * scalar, y * scalar};
}

Vector2 Vector2::operator/(double scalar) const noexcept {
    return {x / scalar, y / scalar};
} 

constexpr double NORMAL_EPSILON = 1e-12;

double Vector2::dot(Vector2 lhs, Vector2 rhs) noexcept {
    return lhs.x * rhs.x + lhs.y * rhs.y;
}

double Vector2::lengthSquared(Vector2 value) noexcept {
    return Vector2::dot(value, value);
}

double Vector2::length(Vector2 value) noexcept {
    return std::sqrt(Vector2::lengthSquared(value));
}

Vector2 Vector2::normalize(Vector2 value) noexcept {
    const double magnitude = Vector2::length(value);

    if (magnitude <= NORMAL_EPSILON)
        return {0.0, 0.0};

    return value * (1.0 / magnitude);
}

Vector2 Vector2::clamp(Vector2 value, Vector2 minimum, Vector2 maximum) noexcept {
    return {
        std::clamp(value.x, minimum.x, maximum.x),
        std::clamp(value.y, minimum.y, maximum.y)
    };
}