#include "utils.h"

#include "setup.h"

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>

#ifndef CHAT_LOG_ERROR
#define CHAT_LOG_ERROR 0
#endif

#ifndef CHAT_LOG_INFO
#define CHAT_LOG_INFO 0
#endif

#if CHAT_LOG_INFO | CHAT_LOG_ERROR
#include <cerrno>
#include <iostream>
#include <mutex>
#endif

void logError([[maybe_unused]] std::string_view source) noexcept {
#if CHAT_LOG_ERROR
    static std::mutex lMutex;
    std::lock_guard{lMutex};

    std::cerr << "ERROR: <" << source << "> " << strerror(errno) << std::endl;
#endif
}

void logInfo([[maybe_unused]] std::string_view info) noexcept {
#if CHAT_LOG_INFO
    static std::mutex lMutex;
    std::lock_guard{lMutex};

    std::cout << "INFO: " << info << std::endl;
#endif
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

int listeningSocket(const ConnectionSetup & setup) noexcept {
    auto socket = ::socket(AF_INET, SOCK_STREAM, 0);
    auto address = getSocketAddress(setup);
    auto casted = reinterpret_cast<sockaddr *>(&address);

    int option = 1;
    if (setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) <
        0)
        logError("setsockopt");

    if (bind(socket, casted, sizeof(address)) < 0) {
        logError("bind");
        close(socket);
        return -1;
    }

    if (listen(socket, 32) < 0) {
        logError("listen");
        close(socket);
        return -1;
    }

    return socket;
}

int connectedSocket(const ConnectionSetup & setup) noexcept {
    auto socket = ::socket(AF_INET, SOCK_STREAM, 0);
    auto address = getSocketAddress(setup);
    auto casted = reinterpret_cast<sockaddr *>(&address);

    if (connect(socket, casted, sizeof(address)) < 0) {
        logError("connect");
        close(socket);
        return -1;
    }

    return socket;
}
