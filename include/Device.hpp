#pragma once

#include <vector>

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
    Device(std::vector<unsigned char> id) : id_(id) {}
    virtual ~Device() = default;

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
    std::vector<unsigned char> id() const {
        return id_;
    }

protected:
    Robot* robot_ = nullptr;

private:
    std::vector<unsigned char> id_;
    
    friend class Robot;
    
};