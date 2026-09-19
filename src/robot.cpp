#include <Robot.hpp>

#include <World.hpp>

#include <utility>

Robot::Robot(World& world, Transform t) : Object(world, t, "Robot") {}

void Robot::deserialize(const nlohmann::json& json) {
    Object::deserialize(json);

    sortedDevices_.clear();
    devices_.clear();

    const auto& devices = json.at("data").at("devices");

    for (const auto& j : devices) {
        auto device = Device::Device_Factory.create(j.at("type").get<std::string>(), j.at("id").get<std::string>());
        if (!device)
            throw std::runtime_error("Device factory returned null");
        
        device->deserialize(j);

        addDevice<Device>(std::move(device));
    }
}

nlohmann::json Robot::serialize() const {
    nlohmann::json json = Object::serialize();
    
    nlohmann::json devices = nlohmann::json::array();
    for (const Device& device : sortedDevices_) {
        devices.push_back(device.serialize());
    }

    json["data"]["devices"] = std::move(devices);
    
    return json;
}

void Robot::registerRobot() {
    Object::Object_Factory.registerType<Robot>("Robot");
}

Device& Robot::addDevice(std::unique_ptr<Device> device) {
    if (!device) 
        throw std::invalid_argument("Cannot add a null device");

    const std::string id = device->id();

    if (devices_.contains(id)) 
        throw std::runtime_error("Device with id '" + id + "' already exists");

    Device* ptr = device.get();

    ptr->robot_ = this;
    ptr->onAttach(*this);

    devices_.emplace(id, std::move(device));

    sortDevices();

    return *ptr;
}

void Robot::update(double deltaTime) {
    if (deltaTime < 0.0) 
        throw std::invalid_argument("Robot update deltaTime cannot be negative");
    
    for (Device& device : sortedDevices_) 
        device.update(deltaTime);
}

void Robot::removeDevice(const std::string& id) {
    auto it = devices_.find(id);

    if (it == devices_.end()) {
        return;
    }

    devices_.erase(it);

    // References into devices_ may now be invalid.
    sortDevices();
}

void Robot::sortDevices() {
    sortedDevices_.clear();
    sortedDevices_.reserve(devices_.size());

    for (const auto& [id, device] : devices_) {
        if (device) {
            sortedDevices_.emplace_back(*device);
        }
    }

    std::ranges::sort(sortedDevices_, {}, [](const Device& device) {
            return device.updatePriority();
        }
    );
}

void Robot::emitChange(const ChangeEvent& event) const {
    world().emit(event);
}

void Robot::emitChange(const std::string& type, const nlohmann::json& data) {
    world().emit(type, data);
}