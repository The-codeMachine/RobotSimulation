#include <Collision.hpp>
#include <Object.hpp>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <functional>
#include <optional>
#include <stdexcept>

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

AcceleratedTrajectory::AcceleratedTrajectory(Vector2 position,
    Vector2 velocity, Vector2 acceleration, double deltaTime)
    : position_(position), velocity_(velocity),
      acceleration_(acceleration), deltaTime_(deltaTime)
{
    if (deltaTime < 0.0)
        throw std::invalid_argument("Trajectory delta time cannot be negative");
}

Vector2 AcceleratedTrajectory::position(double t) const {
    const double time = t * deltaTime_;

    return {
        position_.x
            + velocity_.x * time
            + 0.5 * acceleration_.x * time * time,

        position_.y
            + velocity_.y * time
            + 0.5 * acceleration_.y * time * time
    };
}

Vector2 AcceleratedTrajectory::velocity(double t) const {
    const double time = t * deltaTime_;

    return {
        velocity_.x + acceleration_.x * time,
        velocity_.y + acceleration_.y * time
    };
}

nlohmann::json Collider::serialize() const {
    return {{"type", type_}};
}

AABBCollider::AABBCollider(Vector2 minimum, Vector2 maximum, const std::string& type)
    : Collider(type), minimum_(minimum), maximum_(maximum)
{
    if (minimum.x > maximum.x || minimum.y > maximum.y) {
        throw std::invalid_argument("AABB minimum must not exceed maximum");
    }
}

void AABBCollider::registerAABBCollider() {
    Collider::Collider_Factory.registerType<AABBCollider>("AABBCollider");
}

void AABBCollider::deserialize(const nlohmann::json& json) {
    Collider::deserialize(json);
    
    minimum_.deserialize(json.at("data").at("minimum"));
    maximum_.deserialize(json.at("data").at("maximum"));
}

nlohmann::json AABBCollider::serialize() const {
    nlohmann::json json = Collider::serialize();
    json["data"] = {
        {"minimum", minimum_.serialize()},
        {"maximum", maximum_.serialize()}
    };

    return json;
}

Vector2 AABBCollider::minimum() const noexcept {
    return minimum_;
}

Vector2 AABBCollider::maximum() const noexcept {
    return maximum_;
}

bool AABBCollider::contains(Vector2 point) const {
    return point.x >= minimum_.x
        && point.x <= maximum_.x
        && point.y >= minimum_.y
        && point.y <= maximum_.y;
}

double AABBCollider::signedDistance(Vector2 point) const {
    const Vector2 closest = Vector2::clamp(point, minimum_, maximum_);
    const Vector2 difference = point - closest;

    // Outside the AABB.
    if (difference.x != 0.0 || difference.y != 0.0)
        return Vector2::length(difference);

    // Inside the AABB.
    const double left   = point.x - minimum_.x;
    const double right  = maximum_.x - point.x;
    const double bottom = point.y - minimum_.y;
    const double top    = maximum_.y - point.y;

    return -std::min({left, right, bottom, top});
}

CircleCollider::CircleCollider(Vector2 center, double radius, const std::string& type)
    : Collider(type), center_(center), radius_(radius)
{
    if (radius < 0.0)
        throw std::invalid_argument("Circle radius cannot be negative");
}

void CircleCollider::registerCircleCollider() {
    Collider::Collider_Factory.registerType<CircleCollider>("CircleCollider");
}

void CircleCollider::deserialize(const nlohmann::json& json) {
    Collider::deserialize(json);

    center_.deserialize(json.at("data").at("center"));
    radius_ = json.at("data").at("radius");
}

nlohmann::json CircleCollider::serialize() const {
    nlohmann::json json = Collider::serialize();
    json["data"] = {
        {"center", center_.serialize()},
        {"radius", radius_}
    };

    return json;
}

Vector2 CircleCollider::center() const noexcept {
    return center_;
}

double CircleCollider::radius() const noexcept {
    return radius_;
}

bool CircleCollider::contains(Vector2 point) const {
    return Vector2::lengthSquared(point - center_) <= radius_ * radius_;
}

double CircleCollider::signedDistance(Vector2 point) const {
    return Vector2::length(point - center_) - radius_;
}

std::optional<double> findRoot(const std::function<double(double)>& function,
    double lower, double upper, double epsilon)
{
    if (epsilon <= 0.0)
        throw std::invalid_argument("Root-finding epsilon must be positive");

    if (lower > upper)
        throw std::invalid_argument("Root-finding lower bound must not exceed upper bound");

    double fLower = function(lower);
    const double fUpper = function(upper);

    if (std::abs(fLower) <= epsilon)
        return lower;

    if (std::abs(fUpper) <= epsilon)
        return upper;

    // Bisection requires a sign change.
    if ((fLower > 0.0) == (fUpper > 0.0))
        return std::nullopt;

    while ((upper - lower) > epsilon) {
        const double middle = lower + (upper - lower) * 0.5;
        const double fMiddle = function(middle);

        if (std::abs(fMiddle) <= epsilon)
            return middle;

        if ((fLower > 0.0) == (fMiddle > 0.0)) {
            lower = middle;
            fLower = fMiddle;
        } else {
            upper = middle;
        }
    }

    return lower + (upper - lower) * 0.5;
}

std::optional<CollisionResult> castTrajectory(const Trajectory& trajectory, const Collider& collider,
    Object* object, double epsilon)
{
    constexpr std::size_t SAMPLE_COUNT = 128;

    if (epsilon <= 0.0)
        throw std::invalid_argument("Collision epsilon must be positive");

    double previousT = 0.0;

    double previousDistance = collider.signedDistance(trajectory.position(0.0));

    /*
        The trajectory starts inside or exactly on the collider.

        We deliberately do not invent a collision normal here.
        The generic Collider interface currently only provides
        signedDistance(), which is insufficient to reliably
        determine the surface normal for every possible collider.
    */
    if (previousDistance <= 0.0) {
        return CollisionResult{0.0, trajectory.position(0.0),
            {0.0, 0.0}, object};
    }

    for (std::size_t i = 1; i <= SAMPLE_COUNT; ++i) {
        const double currentT = static_cast<double>(i) / static_cast<double>(SAMPLE_COUNT);

        const Vector2 currentPosition = trajectory.position(currentT);

        const double currentDistance = collider.signedDistance(currentPosition);

        /*
            We crossed from outside to inside.

            Therefore the first collision lies in:
                [previousT, currentT]
        */
        if (currentDistance <= 0.0) {
            const auto root = findRoot(
                [&](double t) {
                    return collider.signedDistance(
                        trajectory.position(t)
                    );
                },
                previousT,
                currentT,
                epsilon
            );

            if (!root)
                return std::nullopt;

            const double collisionTime = *root;

            const Vector2 collisionPosition = trajectory.position(collisionTime);

            /*
                Generic fallback normal.

                This gives the direction of travel rather than
                the actual surface normal, so it should NOT be
                treated as geometrically exact.

                A proper Collider::normalAt(position) method should
                eventually replace this.
            */
            constexpr double NORMAL_DELTA = 1e-7;

            const double beforeT = std::max(0.0, collisionTime - NORMAL_DELTA);
            const double afterT = std::min(1.0, collisionTime + NORMAL_DELTA);

            const Vector2 before = trajectory.position(beforeT);
            const Vector2 after = trajectory.position(afterT);

            const Vector2 normal = Vector2::normalize(after - before);

            return CollisionResult{collisionTime, collisionPosition,
                normal, object};
        }

        previousT = currentT;
        previousDistance = currentDistance;
    }

    return std::nullopt;
}