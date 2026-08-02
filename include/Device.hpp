#pragma once

#include <Factory.hpp>

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
    Device(const std::string& id, const std::string& type) : id_(id), type_(type) {}
    virtual ~Device() = default;

    /// @brief Serializes a device to json
    /// @return the device serialized to json
    virtual nlohmann::json serialize() const {
        return {
            {"id", id_}, 
            {"type", type_},
        };
    }
    
    /// @brief deserializes a device from json
    /// @param json 
    virtual void deserialize(const nlohmann::json& json) {
        id_ = json.at("id");
        type_ = json.at("type");
    }

    /// @brief Called when attached to a robot
    /// @param robot 
    virtual void onAttach(Robot& robot) {}

    /// @brief Called before removal
    virtual void onDetach() {}

    /// @brief Updates this device
    /// @param deltaTime 
    virtual void update(long long deltaTime) = 0;

    /// @brief Gets the id 
    /// @return a copy of the id
    std::string id() const {
        return id_;
    }

protected:
    Robot* robot_ = nullptr;

private:
    std::string id_;
    std::string type_;
    
    friend class Robot;
    
};