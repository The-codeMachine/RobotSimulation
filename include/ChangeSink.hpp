#pragma once

#include <ChangeEvent.hpp>

#include <utility/port.hpp>

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
    void publish(const ChangeEvent& event) override;
};

/// @brief 
///
/// PortChangeSink publishes events to a port. All the
/// information is passed through a port specified at 
/// construction. It is adjustable in runtime. We recommend
/// not adjusting in runtime, but it is possible. 
///
class PortChangeSink : public ChangeSink {
public:
    PortChangeSink(Socket port);

    /// @brief Publishes an event to a port specified at construction
    /// @param event 
    void publish(const ChangeEvent& event) override;

    /// @brief Gets the current Socket being used
    /// @return the current socket being used as a constant reference
    const Socket& port() const;

    /// @brief Updates the port to a new socket. We recommend not using 
    /// this in runtime
    /// @param newPort 
    void updatePort(Socket newPort);

private:
    PortProxy proxy_;

};