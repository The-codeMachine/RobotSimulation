#pragma once

#include <Device.hpp>
#include <Devices/Sensors/SensorShape.hpp>
#include <Devices/Sensors/Image.hpp>

/// @brief Base class for devices that measure properties of the simulation.
class Sensor : public Device {
public:
    Sensor(const std::string& id, const std::string& type = "Sensor");
    Sensor(std::unique_ptr<SensorShape> shape, Transform localTransform, 
        const std::string& id, const std::string& type = "Sensor");

    virtual ~Sensor() = default;

    /// @brief Serializes the Sensor into JSON
    /// @return JSON representing this Sensor
    virtual nlohmann::json serialize() const noexcept override;

    /// @brief Deserializes this Sensor from JSON
    /// @param json 
    virtual void deserialize(const nlohmann::json& json) override;

    /// @brief Gets the shape of the Sensor
    /// @return the shape of the sensor (reference)
    SensorShape& shape();

    /// @brief Gets the shape of the sensor
    /// @return the shape of the sensor (const reference)
    const SensorShape& shape() const noexcept;
    
    /// @brief Gets the transform of the Sensor
    /// @return the transform of the Sensor (reference)
    Transform& localTransform();

    /// @brief Gets the transform of the Sensor
    /// @return the transform of the Sensor (const reference)
    const Transform& localTransform() const noexcept;

    /// @brief Uses the sensor to sense what the specific sensor 
    /// is designed for. Subclasses of the sensor will return 
    /// an object representing it. 
    virtual void sense() = 0;

    void update(double deltaTime) override;

protected:
    std::unique_ptr<SensorShape> shape_;
    Transform localTransform_;

};

/// @brief This is the sensor that views the world around it.
/// This uses the cone SensorShape for detection. Local position
/// is this sensor's position on the Robot. 
class ViewSensor : public Sensor {
public:
    ViewSensor(const std::string& id, const std::string& type = "ViewSensor");
    ViewSensor(double fov, double range, Transform localTransform, 
        const std::string& id, const std::string& type = "ViewSensor");
    ViewSensor(std::unique_ptr<SensorShape> shape, Transform localTransform,
        const std::string& id, const std::string& type = "ViewSensor");

    /// @brief Registers the view sensor to the device factory
    static void registerViewSensor();

    /// @brief Gets the Sensor's global world coordinates
    /// @return the sensor's global world coordinates
    Transform worldTransform() const;

    /// @brief Gets the FOV of the cone
    /// @return the FOV of the cone (reference)
    double& fov();   
    
    /// @brief Gets the FOV of the cone
    /// @return the FOV of the cone (const reference)
    const double& fov() const noexcept;
    
    /// @brief Gets the range of the cone
    /// @return the range of the cone (reference)
    double& range();
        
    /// @brief Gets the range of the cone
    /// @return the range of the cone (const reference)
    const double& range() const noexcept;

    /// @brief Senses an image around it based off the world
    void sense() override;

    /// @brief Gets the last sensed image made by this sensor 
    /// @return the last sensed image made by this sensor (const reference)
    const Image& image() const noexcept;

private:
    Image image_;

};