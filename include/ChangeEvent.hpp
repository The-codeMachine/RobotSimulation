#pragma once

#include <nlohmann/json.hpp>

/// @brief This is the ChangeEvent structure. It represents
/// a new change as an event. It holds which sequence it is
/// part of, what type of change it is, and the data 
/// representing the new values of the change. 
struct ChangeEvent {
    uint64_t sequence;
    std::string type;
    nlohmann::json data;
};