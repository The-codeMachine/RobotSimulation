#include <Device.hpp>

#include <Robot.hpp>
#include <World.hpp>

Device::Device(const std::string& id, const std::string& type)
    : id_(id), type_(type) {}

nlohmann::json Device::serialize() const {
    return {
        {"id", id_}, 
        {"type", type_},
    };
}


void Device::deserialize(const nlohmann::json& json) {
    id_ = json.at("id");
    type_ = json.at("type");
}

std::string Device::id() const {
    return id_;
}

int Device::updatePriority() const noexcept {
    return 0;
}

void Device::emitDeviceChange(const ChangeEvent& event) const {
    if (robot_)
        throw std::runtime_error("Robot is invalid, cannot emit change");
    
    robot_->world().emit(event);
}

void Device::emitDeviceChange(const std::string& type, const nlohmann::json& data) {
    if (robot_)
        throw std::runtime_error("Robot is invalid, cannot emit change");
    
    robot_->world().emit(type, data);
}