#include <ChangeSink.hpp>

void DebugChangeSink::publish(const ChangeEvent &event) {
    std::cout << "Sequence: " << event.sequence << "\n";
    std::cout << "Type: " << event.type << "\n";
    std::cout << "Data: " << event.data.dump() << "\n";
}

PortChangeSink::PortChangeSink(Socket port) : proxy_(port) {}

void PortChangeSink::publish(const ChangeEvent& event) {
    nlohmann::json json;
    json["data"] = event.data;
    json["type"] = event.type;
    json["sequence"] = event.sequence;
    
    proxy_.send(json);
}

std::uint16_t PortChangeSink::port() const {
    return proxy_.port();
}

void PortChangeSink::updatePort(Socket newPort) {
    proxy_.updatePort(newPort);
}