#include <Robot.hpp>

Robot::Robot(World& world, Transform t) : Object(world, t, "Robot") {}

void Robot::deserialize(const nlohmann::json& json) {
    Object::deserialize(json);

    size_t i = 0;
    for (const auto& j : json["data"]["devices"]) {
        devices_.push_back(std::move(Device::Device_Factory.create(j["type"], j["id"])));
        devices_[i]->deserialize(j);
        i++;
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

void Robot::update(long long deltaTime) {
    for (auto& d : devices_) 
        d->update(deltaTime);
}