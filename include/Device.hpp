#pragma once

#include <Factory.hpp>
#include <ChangeEvent.hpp>

#include <nlohmann/json.hpp>

#include <string>

class Robot;
class World;

/// @brief Base class for every robot device.
///
/// Examples:
/// - DifferentialDrive
/// - Camera
/// - Lidar
/// - GPS
/// - Battery
/// - IMU
/// - Encoder
///
class Device {
public:
    static inline Factory::Factory<Device, const std::string&> Device_Factory;

public:
    Device(const std::string& id, const std::string& type);
    virtual ~Device() = default;

    /// @brief Serializes a device to json
    /// @return the device serialized to json
    virtual nlohmann::json serialize() const;
    
    /// @brief deserializes a device from json
    /// @param json 
    virtual void deserialize(const nlohmann::json& json);
    /// @brief Called when attached to a robot
    /// @param robot 
    virtual void onAttach(Robot& robot) {}

    /// @brief Called before removal
    virtual void onDetach() {}

    /// @brief Updates this device
    /// @param deltaTime 
    virtual void update(double deltaTime) = 0;

    /// @brief Gets the id 
    /// @return a copy of the id
    std::string id() const;

    /// @brief Gets and returns the objects update priority. This is so the updates are done sequentially
    /// @return the object's update priority (default = 0)
    virtual int updatePriority() const noexcept {
        return 0;
    }

protected:
    void emitDeviceChange(const ChangeEvent& event) const;
    void emitDeviceChange(const std::string& type, const nlohmann::json& data);

protected:
    Robot* robot_ = nullptr;

private:
    std::string id_;
    std::string type_;
    
    friend class Robot;
    
};