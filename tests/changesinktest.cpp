#include <World.hpp>
#include <Registration.hpp>
#include <ChangeSink.hpp>
#include <ChangeEvent.hpp>
#include <utility/port.hpp>

#include <nlohmann/json.hpp>

#include <cassert>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#ifdef _WIN32
    #include <WinSock2.h>
    #include <WS2tcpip.h>
#else
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <sys/socket.h>
    #include <unistd.h>
#endif


// Test utilities

namespace {

void closeSocket(Socket socket) {
    if (socket == INVALID_SOCKET_VALUE)
        return;

#ifdef _WIN32
    closesocket(socket);
#else
    close(socket);
#endif
}


void sendAll(Socket socket, const void* data, std::size_t size) {
    const auto* bytes = static_cast<const char*>(data);

    while (size > 0) {
#ifdef _WIN32
        const int sent = ::send(
            socket,
            bytes,
            static_cast<int>(size),
            0
        );
#else
        const ssize_t sent = ::send(
            socket,
            bytes,
            size,
            0
        );
#endif

        if (sent <= 0)
            throw std::runtime_error("Failed to send test data");

        bytes += sent;
        size -= static_cast<std::size_t>(sent);
    }
}


void recvAll(Socket socket, void* data, std::size_t size) {
    auto* bytes = static_cast<char*>(data);

    while (size > 0) {
#ifdef _WIN32
        const int received = ::recv(
            socket,
            bytes,
            static_cast<int>(size),
            0
        );
#else
        const ssize_t received = ::recv(
            socket,
            bytes,
            size,
            0
        );
#endif

        if (received <= 0)
            throw std::runtime_error("Failed to receive test data");

        bytes += received;
        size -= static_cast<std::size_t>(received);
    }
}


/**
 * Reads one JSON message from the PortProxy protocol:
 *
 *     [4 byte big-endian length][JSON bytes]
 */
nlohmann::json readFromPort(Socket port) {
    std::uint32_t networkLength = 0;

    recvAll(port, &networkLength, sizeof(networkLength));

    const std::uint32_t length = ntohl(networkLength);

    // Don't let a corrupt length cause an absurd allocation.
    constexpr std::uint32_t MAX_MESSAGE_SIZE = 16 * 1024 * 1024;

    if (length > MAX_MESSAGE_SIZE)
        throw std::runtime_error("Received unreasonable message size");

    std::string data(length, '\0');

    if (length != 0) {
        recvAll(port, data.data(), length);
    }

    return nlohmann::json::parse(data);
}


/**
 * Creates a TCP listener on localhost using an ephemeral port.
 *
 * The operating system chooses the port, which prevents tests from
 * fighting over a hard-coded port such as 5800.
 */
struct TestServer {
    Socket listener = INVALID_SOCKET_VALUE;
    Socket connection = INVALID_SOCKET_VALUE;
    std::uint16_t port = 0;

    TestServer() {
        listener = ::socket(
            AF_INET,
            SOCK_STREAM,
            IPPROTO_TCP
        );

        if (listener == INVALID_SOCKET_VALUE)
            throw std::runtime_error("Could not create test socket");

        int reuse = 1;

        setsockopt(
            listener,
            SOL_SOCKET,
            SO_REUSEADDR,
            reinterpret_cast<const char*>(&reuse),
            sizeof(reuse)
        );

        sockaddr_in address{};
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        address.sin_port = htons(0); // Let OS select a free port.

        if (::bind(
                listener,
                reinterpret_cast<const sockaddr*>(&address),
                sizeof(address)
            ) < 0) {
            closeSocket(listener);
            listener = INVALID_SOCKET_VALUE;
            throw std::runtime_error("Could not bind test socket");
        }

        if (::listen(listener, 1) < 0) {
            closeSocket(listener);
            listener = INVALID_SOCKET_VALUE;
            throw std::runtime_error("Could not listen on test socket");
        }

        sockaddr_in actualAddress{};
#ifdef _WIN32
        int addressSize = sizeof(actualAddress);
#else
        socklen_t addressSize = sizeof(actualAddress);
#endif

        if (::getsockname(
                listener,
                reinterpret_cast<sockaddr*>(&actualAddress),
                &addressSize
            ) < 0) {
            closeSocket(listener);
            listener = INVALID_SOCKET_VALUE;
            throw std::runtime_error("Could not determine test port");
        }

        port = ntohs(actualAddress.sin_port);
    }

    void acceptConnection() {
        connection = ::accept(
            listener,
            nullptr,
            nullptr
        );

        if (connection == INVALID_SOCKET_VALUE)
            throw std::runtime_error("Could not accept test connection");
    }

    ~TestServer() {
        closeSocket(connection);
        closeSocket(listener);
    }

    TestServer(const TestServer&) = delete;
    TestServer& operator=(const TestServer&) = delete;
};


void assertEvent(const nlohmann::json& event, std::uint64_t sequence, const std::string& type, const nlohmann::json& data) {
    assert(event.is_object());

    assert(event.contains("sequence"));
    assert(event.contains("type"));
    assert(event.contains("data"));

    assert(event["sequence"].get<std::uint64_t>() == sequence);
    assert(event["type"].get<std::string>() == type);
    assert(event["data"] == data);
}

} // namespace


// Tests

void testDebugChangeSink() {
    DebugChangeSink sink;

    ChangeEvent event{
        42,
        "test.change",
        {
            {"value", 123},
            {"message", "hello"}
        }
    };

    // Capture stdout.
    std::ostringstream output;
    auto* oldBuffer = std::cout.rdbuf(output.rdbuf());

    sink.publish(event);

    std::cout.rdbuf(oldBuffer);

    const std::string result = output.str();

    // We intentionally don't assert the exact formatting here.
    // Formatting is an implementation detail of DebugChangeSink.
    assert(!result.empty());

    assert(result.find("42") != std::string::npos);
    assert(result.find("test.change") != std::string::npos);
    assert(result.find("123") != std::string::npos);
    assert(result.find("hello") != std::string::npos);

    std::cout << "testDebugChangeSink passed\n";
}


void testPortProxyChangeSink() {
    TestServer server;

    // PortProxy connects immediately in its constructor, so the
    // server must already be listening before PortChangeSink exists.
    PortChangeSink sink(server.port);

    assert(sink.port() == server.port);

    server.acceptConnection();

    ChangeEvent event{
        7,
        "robot.transform",
        {
            {"x", 10.5},
            {"y", -2.0},
            {"rotation", 90.0}
        }
    };

    sink.publish(event);

    const nlohmann::json received = readFromPort(server.connection);

    assertEvent(
        received,
        event.sequence,
        event.type,
        event.data
    );

    std::cout << "testPortProxyChangeSink passed\n";
}


void testWorldIntegration() {
    TestServer server;

    // Construct the sink before the World so the World can immediately
    // use it for emitted events.
    auto sink = std::make_unique<PortChangeSink>(server.port);

    World world(std::filesystem::path("assets/tests/changeSinkTest.json"));

    world.setSink(std::move(sink));

    server.acceptConnection();

    const nlohmann::json data{
        {"x", 10},
        {"y", 20}
    };

    world.emit("test.world.change", data);

    const nlohmann::json received = readFromPort(server.connection);

    assert(received.is_object());
    assert(received.contains("sequence"));
    assert(received.contains("type"));
    assert(received.contains("data"));

    assert(received["type"].get<std::string>() == "test.world.change");

    assert(received["data"] == data);

    // The first event emitted by this World should have sequence 0
    // or 1 depending on your intended sequence convention.
    //
    // If your implementation starts sequences at 0:
    assert(received["sequence"].get<std::uint64_t>() == 0);

    std::cout << "testWorldIntegration passed\n";
}


// Main

int main() {
    registerBuiltinObjects();

#ifdef _WIN32
    WSADATA wsaData{};

    const int result = WSAStartup(
        MAKEWORD(2, 2),
        &wsaData
    );

    if (result != 0) {
        std::cerr << "WSAStartup failed: " << result << '\n';
        return 1;
    }
#endif

    try {
        testDebugChangeSink();
        testPortProxyChangeSink();
        testWorldIntegration();

#ifdef _WIN32
        WSACleanup();
#endif

        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << '\n';

#ifdef _WIN32
        WSACleanup();
#endif

        return 1;
    }
    catch (...) {
        std::cerr << "unknown exception\n";

#ifdef _WIN32
        WSACleanup();
#endif

        return 1;
    }
}