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
/// Ids are only unique to the specific Device type,
/// there can be multiple devices with the same id,
/// as long as they are different device subclasses. 
///
class Robot : public Object {
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
        return addDevice<T>(std::make_unique<T>(id, std::forward<Args>(args)...));
    }

    /// @brief Adds a device based off a constructed unique pointer
    /// @tparam T 
    /// @param device 
    /// @return a reference to that device 
    template<typename T>
    T& addDevice(std::unique_ptr<Device> device) {
        if (getDevice<T>(device->id()) != nullptr)
            return nullptr;

        T* ptr = dynamic_cast<T*>(device.get());
        devices_.push_back(std::move(device));

        ptr->robot_ = this;
        ptr->onAttach(*this);

        return *ptr;
    }

    /// @brief Gets device T. Returns nullptr if it does not exist
    /// @tparam T 
    /// @param id
    /// @return device T if it exists
    template<typename T> 
    T* getDevice(const std::vector<unsigned char>& id) const {
        for (const auto& d : devices_) {
            T* device = dynamic_cast<T*>(d.get());
            
            if (device && device->id() == id) 
                return device;
        }

        return nullptr;
    }

    /// @brief This updates the robot based off deltaTime 
    /// @param deltaTime 
    void update(long long deltaTime);

private:
    std::vector<std::unique_ptr<Device>> devices_;

};