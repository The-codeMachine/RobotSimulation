#pragma once

#include <Device.hpp>
#include <Object.hpp>

#include <ChangeEvent.hpp>

#include <algorithm>
#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

class World;

/// @brief
///
/// Encapsulates an Object that can move, see, and contain
/// any number of Devices. Devices provide the Robot's
/// functionality; Robot itself does not implement specific
/// functionality such as movement or vision.
///
/// Device IDs are unique within a Robot.
class Robot : public Object {
public:
    Robot(World& world, Transform t);

    void deserialize(const nlohmann::json& json) override;
    nlohmann::json serialize() const override;

    /// @brief Registers the robot to Objects. 
    static void registerRobot();

    /// @brief Constructs and adds a device to the Robot
    /// @tparam T 
    /// @tparam ...Args 
    /// @param id
    /// @param ...args 
    /// @return a reference to the device constructed
    template<typename T, typename... Args>
    T& addDevice(const std::string& id, Args&&... args) {
        return addDevice<T>(std::make_unique<T>(id, std::forward<Args>(args)...));
    }

    /// @brief Adds a device based off a constructed unique pointer
    /// @tparam T 
    /// @param device 
    /// @return a reference to that device 
    template<typename T>
    T& addDevice(std::unique_ptr<Device> device) {
        if (!device)
            throw std::invalid_argument("Cannot add a null device");

        T* ptr = dynamic_cast<T*>(device.get());
        if (!ptr)
            throw std::invalid_argument("Device type does not match template parameter");

        const std::string id = device->id();
        if (devices_.contains(id)) 
            throw std::runtime_error("Device with id '" + id + "' already exists");
        
        ptr->robot_ = this;
        ptr->onAttach(*this);

        devices_.emplace(id, std::move(device));
        
        sortDevices();

        return *ptr;
    }

    /// @brief Adds a device based off a constructed unique pointer
    /// @param device 
    /// @return a device reference to the object that was inserted
    Device& addDevice(std::unique_ptr<Device> device);

    /// @brief Gets device T by ID. Returns nullptr if it does not exist
    /// or if the device is not of type T.
    /// @tparam T 
    /// @param id
    /// @return device T if it exists
    template<typename T> 
    T* getDevice(const std::string& id) {
        auto it = devices_.find(id);

        if (it == devices_.end() || !it->second) {
            return nullptr;
        }

        return dynamic_cast<T*>(it->second.get());
    }

    /// @brief Gets device T by ID. Returns nullptr if it does not exist
    /// or if the device is not of type T.
    /// @tparam T
    /// @param id
    /// @return device T if it exists (a constant pointer)
    template<typename T>
    const T* getDevice(const std::string& id) const {
        auto it = devices_.find(id);

        if (it == devices_.end() || !it->second) {
            return nullptr;
        }

        return dynamic_cast<const T*>(it->second.get());
    }

    /// @brief Removes a device from the robot. 
    /// @param id 
    void removeDevice(const std::string& id);

    /// @brief This updates the robot based off deltaTime 
    /// @param deltaTime 
    void update(double deltaTime);

    /// @brief Emits an event to the World by an Event
    /// @param event 
    void emitChange(const ChangeEvent& event) const;

    /// @brief Emits an event to the World by specifying the type and the data of the event
    /// @param type 
    /// @param data 
    void emitChange(const std::string& type, const nlohmann::json& data);

private:
    void sortDevices();

private:
    std::unordered_map<std::string, std::unique_ptr<Device>> devices_;
    std::vector<std::reference_wrapper<Device>> sortedDevices_;

};