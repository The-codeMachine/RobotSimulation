#include <Robot.hpp>

#include <stdexcept>

Robot::Robot(const std::vector<Device>& devices) : devices_(devices) {}

Device& Robot::getDevice(uint32_t deviceId) {
    if (!validIndex(deviceId))
        throw std::runtime_error("Index is out of range for this robot");

    return devices_[deviceId];
}

const Device& Robot::getDevice(uint32_t deviceId) const {
    if (!validIndex(deviceId))
        throw std::runtime_error("Index is out of range for this robot");

    return devices_[deviceId];
}

bool Robot::validIndex(uint32_t i) const noexcept {
    return i < devices_.size();
}