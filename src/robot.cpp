#include <Robot.hpp>

#include <cmath>

Robot::Robot(World& world, Transform t) : Object(world, t, "Robot") {}

void Robot::deserialize(const nlohmann::json& json) {
    Object::deserialize(json);

    devices_.clear();

    for (const auto& j : json.at("data").at("devices")) {
        auto device = Device::Device_Factory.create(std::string_view(j.at("type")), j.at("id"));
        device->deserialize(j);

        addDevice<Device>(std::move(device));
    }
}

nlohmann::json Robot::serialize() const {
    nlohmann::json json = Object::serialize();
    
    nlohmann::json data = nlohmann::json::array();
    for (const auto& d : devices_) {
        if (d == nullptr)
            continue;

        data.push_back(d->serialize());
    }

    json["data"]["devices"] = data;
    
    return json;
}

void Robot::registerRobot() {
    Object::Object_Factory.registerType<Robot>("Robot");
}

Device& Robot::addDevice(std::unique_ptr<Device> device) {
    return addDevice<Device>(std::move(device));
}

void Robot::update(double deltaTime) {
    for (auto& d : devices_) 
        d->update(deltaTime);
}

void Robot::sortDevices() {
    std::ranges::sort(devices_, {}, &Device::updatePriority);
}