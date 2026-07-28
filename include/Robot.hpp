#pragma once

#include <Device.hpp>
#include <Object.hpp>

#include <vector>
#include <memory>
#include <stdexcept>

class World;

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
class Robot : Object {
public:
    Robot(World& world, Transform t);

    /// @brief Constructs and adds a device to the Robot
    /// @tparam T 
    /// @tparam ...Args 
    /// @param id
    /// @param ...args 
    /// @return a reference to the device constructed
    template<typename T, typename... Args>
    T& addDevice(const std::vector<unsigned char>& id, Args&&... args) {
        addDevice<T>(std::make_unique<Device>(id, args));
    }

    template<typename T>
    T& addDevice(std::unique_ptr<Device> device) {
        if (getDevice<T>(device->id()))
            throw std::runtime_error("Device with that id already exists");

        devices_.push_back(device);
    }

    /// @brief Gets device T. Throws if it does not exist
    /// @tparam T 
    /// @param id
    /// @return device T if it exists
    template<typename T> 
    T* getDevice(const std::vector<unsigned char>& id) {
        for (const auto& d : devices_) {
            T* device = dynamic_cast<T*>(d.get());
            
            if (device && device->id() == id) 
                return device;
        }
    }

    /// @brief This updates the robot based off deltaTime 
    /// @param deltaTime 
    void update(long long deltaTime);

private:
    std::vector<std::unique_ptr<Device>> devices_;

};