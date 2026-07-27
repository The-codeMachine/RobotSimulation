#pragma once

#include <Device.hpp>

#include <vector>

/// @brief 
///
/// This is the Robot class. It encapsulates
/// an object that can move, see, and have
/// any other number of devices. These devices
/// give the Robot specific functionality. The
/// robot simply encapsulates these devices. 
/// The robot does not support functionality like
/// movement, or view. Devices support this.
///
class Robot {
public:
    Robot() = default;
    Robot(const std::vector<Device>& devices);

    /// @brief Gets the device located at this index
    /// @param deviceId 
    /// @return the device located at this index
    Device& getDevice(uint32_t deviceId);

    /// @brief Gets the device located at this index
    /// @param deviceId 
    /// @return the device located at this index
    const Device& getDevice(uint32_t deviceId) const;

    /// @brief This updates the robot based off deltaTime 
    /// (IDK how I am going to implement it, maybe update 
    /// all devices, and devices will have a custom function to handle that)
    /// @param deltaTime 
    void update(long long deltaTime);

private:
    /// @brief Checks whether i is a valid index
    /// @param i 
    /// @return true if i is a valid index
    bool validIndex(uint32_t i) const noexcept;

private:
    std::vector<Device> devices_;

};