#pragma once

/// @brief 
///
/// This is the Super class of all devices.
/// It encapsulates nothing, but gives all
/// devices some basic functionality, and
/// permissions. Most subclasses are completely 
/// independent of this, but needs this. 
///
class Device {
public:
    Device();

    void update(long long deltaTime);
};