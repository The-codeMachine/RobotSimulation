#pragma once

#include <Object.hpp>

/// @brief Sensor shape is a specific geometric
/// shape specifying how a sensor scans an area
/// in World. It holds a simple origin point 
/// transform. 
class SensorShape {
public:
    static inline Factory::Factory<SensorShape, Transform> SensorShape_Factory;

public:
    SensorShape(const std::string& type = "SensorShape");
    SensorShape(Transform origin, const std::string& type = "SensorShape");

    virtual ~SensorShape() = default;

    /// @brief Serializes a SensorShape 
    /// @return JSON representing a base SensorShape (returning its origin)
    virtual nlohmann::json serialize() const noexcept;

    /// @brief Based off the inputted json, this will deconstruct the origin and set it
    /// @param json 
    virtual void deserialize(const nlohmann::json& json);

    virtual bool contains(Transform point) const noexcept = 0;

    /// @brief Gets the origin
    /// @return the origin as a reference
    Transform& origin();
    
    /// @brief Gets the origin
    /// @return the origin as a const reference
    const Transform& origin() const noexcept;

protected:
    Transform origin_;

private:
    std::string type_;

};

/// @brief This is a subclass of SensorShape. It 
/// inherits publically from SensorShape. It defines
/// a quadratic shape representing the SensorShape. 
/// This can be any shape with 4 points. This is 
/// not based off the origin. 
class SensorShapeQuadratic : public SensorShape {
public:
    SensorShapeQuadratic(Transform origin, const std::string& type = "SensorShapeQuadratic");
    SensorShapeQuadratic(const std::array<Transform, 4>& vertices, const std::string& type = "SensorShapeQuadratic");
    
    /// @brief Registers SensorShapeQuadratic into the SensorShape Factory
    static void registerSensorShapeQuadratic();

    /// @brief Serializes the SensorShapeQuadratic into JSON
    /// @return JSON representing this object
    nlohmann::json serialize() const noexcept override;

    /// @brief Deserializes the SensorShapeQuadratic from JSON
    /// @param json 
    void deserialize(const nlohmann::json& json) override;

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
    static inline constexpr size_t NUMBER_OF_VERTICES = 4;

private:
    /// @brief Orders all the vertices counter-clockwise around the center
    /// @param vertices 
    /// @return the ordered array of vertices
    static std::array<Transform, NUMBER_OF_VERTICES> order_vertices_(const std::array<Transform, 4>& vertices);

private:
    std::array<Transform, NUMBER_OF_VERTICES> vertices_;


};

/// @brief This is a subclass of SensorShape. It
/// inherits publically from SensorShape. It defines
/// a cone shape. This cone shape is defined by an
/// FOV (field of view) and range (how far the cone
/// expands for view). This is based off the origin. 
class SensorShapeCone : public SensorShape {
public:
    SensorShapeCone(Transform origin, const std::string& type = "SensorShapeCone");
    SensorShapeCone(Transform origin, double fov, double range, const std::string& type = "SensorShapeCone");
    
    /// @brief Registers SensorShapeCone into the SensorShape Factory
    static void registerSensorShapeCone();
    
    /// @brief Serializes the SensorShapeCone into JSON
    /// @return JSON representing this object
    nlohmann::json serialize() const noexcept override;

    /// @brief Deserializes the SensorShapeCone from JSON
    /// @param json 
    void deserialize(const nlohmann::json& json) override;

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
    SensorShapeBall(Transform origin, const std::string& type = "SensorShapeBall");
    SensorShapeBall(Transform origin, double radius, const std::string& type = "SensorShapeBall");

    /// @brief Registers SensorShapeBall into the SensorShape Factory
    static void registerSensorShapeBall();

    /// @brief Serializes the SensorShapeBall into JSON
    /// @return JSON representing this object
    nlohmann::json serialize() const noexcept override;

    /// @brief Deserializes the SensorShapeBall from JSON
    /// @param json 
    void deserialize(const nlohmann::json& json) override;

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