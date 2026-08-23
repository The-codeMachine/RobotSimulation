#pragma once

#include <Factory.hpp>
#include <Vector2.hpp>

#include <optional>
#include <functional>
#include <limits>

class Object;

/// @brief A normalized trajectory through a single physics timestep.
///
/// t = 0 represents the beginning of the timestep
/// t = 1 represents the end of the timestep
///
/// The trajectory can be linear, accelerated, or eventually any other
/// parametic path. 
class Trajectory {
public:
    virtual ~Trajectory() = default;

    /// @brief Gets the position at normalized time t
    /// @param t Normalized time in [0, 1]
    /// @return the position at normalized time t 
    virtual Vector2 position(double t) const = 0;
    
    /// @brief Gets the velocity at normalized time t
    /// @param t Normalized time in [0, 1]
    /// @return the velocity at normalized time t
    virtual Vector2 velocity(double t) const = 0;
};

/// @brief A trajectory produced by constant acceleration.
///
/// p(t) = p0 + v0 * t + 0.5 * a * t^2
///
/// where t is the actual elapsed time represented by the
/// normalized parameter.
class AcceleratedTrajectory final : public Trajectory {
public:
    AcceleratedTrajectory(Vector2 position, Vector2 velocity, Vector2 acceleration, double deltaTime);

    Vector2 position(double t) const override;
    Vector2 velocity(double t) const override;

private:
    Vector2 position_;
    Vector2 velocity_;
    Vector2 acceleration_;
    double deltaTime_;

};

/// @brief Base class for all collision shapes
class Collider {
public:
    static inline Factory::Factory<Collider> Collider_Factory;

public:
    Collider(const std::string& type) : type_(type) {} 

    virtual ~Collider() = default;

    virtual void deserialize(const nlohmann::json& json) {}
    virtual nlohmann::json serialize() const;

    /// @brief Determines whether a point is inside the collider
    /// @param point 
    /// @return true if the point is inside the collisde
    virtual bool contains(Vector2 point) const = 0;

    /// @brief Returns the signed distance from a point to the collider
    ///
    /// Negative = inside
    /// Zero = on the surface
    /// Positive = outside
    ///
    /// @param point 
    /// @return the signed distance from a point to a collider
    virtual double signedDistance(Vector2 point) const = 0;

private:
    std::string type_;

};

/// @brief Axis-aligned bounding box collider
class AABBCollider final : public Collider {
public:
    AABBCollider(Vector2 minimum = {0, 0}, Vector2 maximum = {0, 0}, const std::string& type = "AABBCollider");

    static void registerAABBCollider();

    /// @brief Deserializes this collider based off JSON
    /// @param json 
    void deserialize(const nlohmann::json& json) override;

    /// @brief Serializes this collider to JSON
    /// @return this collider represented as JSON
    nlohmann::json serialize() const override;

    /// @brief Gets the minimum Vector of this AABB collider
    /// @return the minimum vector of this AABB collider
    Vector2 minimum() const noexcept;

    /// @brief Gets the maximum Vector of this AABB collider
    /// @return the maximum vector of this AABB collider
    Vector2 maximum() const noexcept;

    /// @brief Checks whether this point is within this AABB collider
    /// @param point 
    /// @return true if the pointer is within this collider
    bool contains(Vector2 point) const override;

    /// @brief Calculates the signed distance from the collider to this point
    /// @param point 
    /// @return the signed distance from the collider to this point 
    double signedDistance(Vector2 point) const override;

private:
    Vector2 minimum_;
    Vector2 maximum_;

};

/// @brief A circular collider
class CircleCollider final : public Collider {
public:
    CircleCollider(Vector2 center = {0, 0}, double radius = 1, const std::string& type = "CircleCollider");

    static void registerCircleCollider();

    /// @brief Deserializes this collider based off JSON
    /// @param json 
    void deserialize(const nlohmann::json& json) override;

    /// @brief Serializes this collider to JSON
    /// @return this collider represented as JSON
    nlohmann::json serialize() const override;

    /// @brief Gets the center position of the circle of this collider
    /// @return Vector2 position representing the center of the circle
    Vector2 center() const noexcept;

    /// @brief Gets the radius of the circle of this collider
    /// @return the radius of this circle
    double radius() const noexcept;

    /// @brief Checks whether this point is within this circle collider
    /// @param point 
    /// @return true if the pointer is within this collider
    bool contains(Vector2 point) const override;

    /// @brief Calculates the signed distance from the collider to this point
    /// @param point 
    /// @return the signed distance from the collider to this point 
    double signedDistance(Vector2 point) const override;

private:
    Vector2 center_;
    double radius_;

};

/// @brief  Result of a successful collision cast
struct CollisionResult {
    /// @brief Normalized time at which the collision occurred.
    ///
    /// 0 = beginning of trajectory
    /// 1 = end of trajectory
    double time;

    /// @brief Position of the collision
    Vector2 position;

    /// @brief Surface normal at the collision point
    Vector2 normal;

    /// @brief Object that was collided with
    Object* object;
};

/// @brief Numerical root finder.
///
/// Searches for a root of f(t) between lower and upper.
/// The function values at the bounds must have opposite signs,
/// or one of the bounds must already be a root.
/// @param function 
/// @param lower 
/// @param upper 
/// @param epsilon 
/// @return the numeric root of two values 
std::optional<double> findRoot(const std::function<double(double)>& function, 
    double lower, double upper, double epsilon = 1e-10);

/// @brief Casts a trajectory against a collider.
///
/// Returns the first collision occurring during t in [0, 1].
///
/// The object parameter identifies what was collided with and may
/// be nullptr when performing a geometry-only test.
/// @param trajectory 
/// @param collider 
/// @param object 
/// @param epsilon 
/// @return 
std::optional<CollisionResult> castTrajectory(const Trajectory& trajectory, const Collider& collider, 
    Object* object = nullptr, double epsilon = 1e-10);