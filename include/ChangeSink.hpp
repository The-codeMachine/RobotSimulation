#pragma once

#include <ChangeEvent.hpp>

#include <iostream>

/// @brief This is a sink representing a change to an object.
/// It can publish those changes. This is simply a base class
/// an provides little-to-no implementation for publish functions
/// or an other functions. 
///
/// A ChangeSink will simply do something with an event, whether
/// that be printing, or publishing it to a port.  
class ChangeSink {
public:
    virtual ~ChangeSink() = default;

    /// @brief This publishes a type of event. Currently, this does nothing. 
    /// @param event 
    virtual void publish(const ChangeEvent& event) = 0;
};

/// @brief This is a simple ChangeSink used for debugging. It will
/// print the event's data. This includes:
/// - its sequence
/// - its JSON data
/// - its type
class DebugChangeSink : public ChangeSink {
public:
    /// @brief Prints the event's data to the console
    /// @param event 
    void publish(const ChangeEvent& event) override {
        std::cout << "Sequence: " << event.sequence << "\n";
        std::cout << "Type: " << event.type << "\n";
        std::cout << "Data: " << event.data.dump() << "\n";
    }
};