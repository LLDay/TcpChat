#include "utils.h"

#include "setup.h"

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <mutex>

void logError(std::string_view source) noexcept {
    static std::mutex lMutex;
    std::lock_guard{lMutex};

    std::cerr << "ERROR: <" << source << "> " << strerror(errno) << std::endl;
}

void logInfo(std::string_view info) noexcept {
    static std::mutex lMutex;
    std::lock_guard{lMutex};

    std::cout << "INFO: " << info << std::endl;
}

void makeNonBlocking(int fd) noexcept {
    auto flags = fcntl(fd, F_GETFL);
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0)
        logError("fcntl");
}

sockaddr_in getSocketAddress(const ConnectionSetup & setup) {
    sockaddr_in address;
    memset(&address, 0, sizeof(address));

    address.sin_family = AF_INET;
    address.sin_port = htons(setup.port);

    if (inet_pton(AF_INET, setup.address.c_str(), &address.sin_addr) <= 0)
        logError("inet_pton");

    return address;
}

int bindedSocket(const ConnectionSetup & setup) noexcept {
    auto socket = ::socket(AF_INET, SOCK_STREAM, 0);
    auto address = getSocketAddress(setup);
    auto casted = reinterpret_cast<sockaddr *>(&address);

    if (bind(socket, casted, sizeof(address)) < 0)
        logError("bind");

    return socket;
}

int connectedSocket(const ConnectionSetup & setup) noexcept {
    auto socket = ::socket(AF_INET, SOCK_STREAM, 0);
    auto address = getSocketAddress(setup);
    auto casted = reinterpret_cast<sockaddr *>(&address);

    if (connect(socket, casted, sizeof(address)) < 0)
        logError("connect");

    return socket;
}
