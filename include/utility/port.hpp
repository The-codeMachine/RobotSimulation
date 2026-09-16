#pragma once

#include <nlohmann/json.hpp>

#include <cstddef>
#include <cstdint>
#include <stdexcept>

#ifdef _WIN32
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif

    #include <WinSock2.h>
    #include <WS2tcpip.h>

    using Socket = SOCKET;
    constexpr Socket INVALID_SOCKET_VALUE = INVALID_SOCKET;

#else
    #include <arpa/inet.h>
    #include <cerrno>
    #include <netinet/in.h>
    #include <sys/socket.h>
    #include <unistd.h>

    using Socket = int;
    constexpr Socket INVALID_SOCKET_VALUE = -1;
#endif

/// @brief
/// A lightweight one-way TCP proxy for sending raw data or JSON to
/// a process listening on localhost.
///
/// JSON messages are sent using a 4-byte network-order length prefix
/// followed by the serialized JSON data.
class PortProxy {
public:
    /// @brief Constructs the PortProxy with a default port of 5800.
    /// @param port The localhost TCP port to connect to.
    explicit PortProxy(std::uint16_t port = 5800);

    /// @brief Closes the connection.
    ~PortProxy();

    PortProxy(const PortProxy&) = delete;
    PortProxy& operator=(const PortProxy&) = delete;

    PortProxy(PortProxy&& other) noexcept;
    PortProxy& operator=(PortProxy&& other) noexcept;

    /// @brief Sends raw data through the port.
    /// @param dataSize Number of bytes to send.
    /// @param data Pointer to the data.
    ///
    /// @throws std::invalid_argument if data is null while dataSize > 0.
    /// @throws std::runtime_error if the data cannot be sent.
    void send(std::size_t dataSize, const void* data);

    /// @brief Sends JSON over the port.
    /// @param json JSON to serialize and send.
    void send(const nlohmann::json& json);

    /// @brief Gets the port currently being used.
    /// @return The TCP port.
    std::uint16_t port() const noexcept;

    /// @brief Changes the destination port and reconnects.
    /// @param newPort The new localhost TCP port.
    void updatePort(std::uint16_t newPort);

    /// @brief Returns whether the proxy currently has a TCP connection.
    bool isConnected() const noexcept;

private:
    /// @brief Connects to localhost on port_.
    void setupProxy_();

    /// @brief Closes the socket if it is open.
    void closeSocket_() noexcept;

    /// @brief Sends every byte in the supplied buffer.
    void sendAll_(const void* data, std::size_t size);

private:
    std::uint16_t port_;
    Socket socket_ = INVALID_SOCKET_VALUE;
};