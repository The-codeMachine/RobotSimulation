#pragma once

#include <Object.hpp>

/// @brief Sensor shape is a specific geometric
/// shape specifying how a sensor scans an area
/// in World. It holds a simple origin point 
/// transform. 
class SensorShape {
public:
    SensorShape(Transform origin);

    virtual bool contains(Transform point) const noexcept = 0;

    /// @brief Gets the origin
    /// @return the origin as a reference
    Transform& origin();
    
    /// @brief Gets the origin
    /// @return the origin as a const reference
    const Transform& origin() const noexcept;

protected:
    Transform origin_;

};

/// @brief This is a subclass of SensorShape. It 
/// inherits publically from SensorShape. It defines
/// a quadratic shape representing the SensorShape. 
/// This can be any shape with 4 points. This is 
/// not based off the origin. 
class SensorShapeQuadratic : public SensorShape {
public:
    SensorShapeQuadratic(const std::array<Transform, 4>& vertices);
    
    /// @brief Gets the vertices
    /// @return the vertices as a const reference
    const std::array<Transform, 4>& vertices() const noexcept;

    /// @brief Sets the vertices to a new array
    /// @param vertices 
    void setVertices(const std::array<Transform, 4>& vertices);

    /// @brief Checks whether a point is on the quadratic 
    /// @param point 
    /// @return true if the point is within the quadratic
    bool contains(Transform point) const noexcept override;

private:
    /// @brief Orders all the vertices counter-clockwise around the center
    /// @param vertices 
    /// @return the ordered array of vertices
    static std::array<Transform, 4> order_vertices_(const std::array<Transform, 4>& vertices);

private:
    std::array<Transform, 4> vertices_;

};

/// @brief This is a subclass of SensorShape. It
/// inherits publically from SensorShape. It defines
/// a cone shape. This cone shape is defined by an
/// FOV (field of view) and range (how far the cone
/// expands for view). This is based off the origin. 
class SensorShapeCone : public SensorShape {
public:
    SensorShapeCone(Transform origin, double fov, double range);

    /// @brief Gets the FOV
    /// @return the FOV as a reference
    double& fov();
    
    /// @brief Gets the FOV
    /// @return the FOV as a const reference
    const double& fov() const noexcept;

    /// @brief Gets the range
    /// @return the range as a reference
    double& range();
    
    /// @brief Gets the range
    /// @return the range as a const reference
    const double& range() const noexcept;

    /// @brief Checks whether a point is on the cone
    /// @param point 
    /// @return true if the point is within the cone 
    bool contains(Transform point) const noexcept override; 

private:
    double fov_;
    double range_;

};

/// @brief This is a subclass of SensorShape. It 
/// inherits publically from SensorShape. It defines
/// a ball shape around origin. This shape is based
/// off a radius. 
class SensorShapeBall : public SensorShape {
public:
    SensorShapeBall(Transform origin, double radius);

    /// @brief Gets the radius
    /// @return the radius as a reference
    double& radius();
    
    /// @brief Gets the radius
    /// @return the radius as a const reference
    const double& radius() const noexcept;

    /// @brief Checks whether a point is on the ball 
    /// @param point 
    /// @return true if the point is within the ball 
    bool contains(Transform point) const noexcept override;

private:
    double radius_;

};