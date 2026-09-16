#include <utility/port.hpp>

#include <cstring>
#include <limits>
#include <string>

#ifdef _WIN32
    #pragma comment(lib, "Ws2_32.lib")
#endif

namespace {

#ifdef _WIN32

class WinsockInitializer {
public:
    WinsockInitializer() {
        WSADATA data{};

        const int result = WSAStartup(MAKEWORD(2, 2), &data);

        if (result != 0) {
            throw std::runtime_error("WSAStartup failed");
        }
    }

    ~WinsockInitializer() {
        WSACleanup();
    }

    WinsockInitializer(const WinsockInitializer&) = delete;
    WinsockInitializer& operator=(const WinsockInitializer&) = delete;
};

void closeSocket(Socket socket) noexcept {
    if (socket != INVALID_SOCKET_VALUE) {
        closesocket(socket);
    }
}

#else

void closeSocket(Socket socket) noexcept {
    if (socket != INVALID_SOCKET_VALUE) {
        ::close(socket);
    }
}

#endif

} // namespace

PortProxy::PortProxy(std::uint16_t port)
    : port_(port) {

#ifdef _WIN32
    // WSAStartup must happen before socket functions are used.
    static WinsockInitializer winsock;
#endif

    setupProxy_();
}

PortProxy::~PortProxy() {
    closeSocket_();
}

PortProxy::PortProxy(PortProxy&& other) noexcept
    : port_(other.port_), socket_(other.socket_) {

    other.socket_ = INVALID_SOCKET_VALUE;
}

PortProxy& PortProxy::operator=(PortProxy&& other) noexcept {
    if (this == &other) {
        return *this;
    }

    closeSocket_();

    port_ = other.port_;
    socket_ = other.socket_;

    other.socket_ = INVALID_SOCKET_VALUE;

    return *this;
}

void PortProxy::send(std::size_t dataSize, const void* data) {
    if (dataSize > 0 && data == nullptr) {
        throw std::invalid_argument("PortProxy::send received null data with non-zero size");
    }

    if (dataSize > std::numeric_limits<std::uint32_t>::max()) {
        throw std::length_error("PortProxy message is too large");
    }

    if (socket_ == INVALID_SOCKET_VALUE) {
        setupProxy_();
    }

    // 4-byte network-order message length.
    const std::uint32_t messageSize = static_cast<std::uint32_t>(dataSize);
    const std::uint32_t networkSize = htonl(messageSize);

    sendAll_(&networkSize, sizeof(networkSize));

    if (dataSize > 0) {
        sendAll_(data, dataSize);
    }
}

void PortProxy::send(const nlohmann::json& json) {
    const std::string serialized = json.dump();

    send(serialized.size(), serialized.data());
}

std::uint16_t PortProxy::port() const noexcept {
    return port_;
}

void PortProxy::updatePort(std::uint16_t newPort) {
    if (newPort == port_) {
        return;
    }

    closeSocket_();

    port_ = newPort;

    setupProxy_();
}

bool PortProxy::isConnected() const noexcept {
    return socket_ != INVALID_SOCKET_VALUE;
}

void PortProxy::setupProxy_() {
    closeSocket_();

    socket_ = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (socket_ == INVALID_SOCKET_VALUE) {
        throw std::runtime_error("Failed to create TCP socket");
    }

    sockaddr_in address{};

    address.sin_family = AF_INET;
    address.sin_port = htons(port_);

    // localhost
    const int result = inet_pton(
        AF_INET,
        "127.0.0.1",
        &address.sin_addr
    );

    if (result != 1) {
        closeSocket_();

        throw std::runtime_error("Failed to create localhost address");
    }

    const int connectResult = ::connect(
        socket_,
        reinterpret_cast<const sockaddr*>(&address),
        sizeof(address)
    );

    if (connectResult != 0) {
        closeSocket_();

        throw std::runtime_error("Failed to connect to localhost port " + std::to_string(port_));
    }
}

void PortProxy::closeSocket_() noexcept {
    closeSocket(socket_);

    socket_ = INVALID_SOCKET_VALUE;
}

void PortProxy::sendAll_(const void* data, std::size_t size) {
    const char* bytes = static_cast<const char*>(data);

    std::size_t totalSent = 0;

    while (totalSent < size) {

#ifdef _WIN32
        const int chunkSize = static_cast<int>(
            std::min<std::size_t>(size - totalSent, 
                static_cast<std::size_t>(std::numeric_limits<int>::max()))
        );

        const int sent = ::send(
            socket_,
            bytes + totalSent,
            chunkSize,
            0
        );
#else
        const std::size_t chunkSize =
            std::min<std::size_t>(
                size - totalSent,
                static_cast<std::size_t>(
                    std::numeric_limits<ssize_t>::max()
                )
            );

        const ssize_t sent = ::send(
            socket_,
            bytes + totalSent,
            chunkSize,
            0
        );
#endif

        if (sent <= 0) {
            closeSocket_();

            throw std::runtime_error("Failed to send data through PortProxy");
        }

        totalSent += static_cast<std::size_t>(sent);
    }
}