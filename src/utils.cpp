#include "utils.h"

#include "setup.h"

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <optional>

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

std::optional<sockaddr_in> getSocketAddress(const ConnectionSetup & setup) {
    sockaddr_in address;
    memset(&address, 0, sizeof(address));

    address.sin_family = AF_INET;
    address.sin_port = htons(setup.port);

    if (inet_pton(AF_INET, setup.address.c_str(), &address.sin_addr) <= 0)
        return std::nullopt;

    return address;
}

ConnectionResponse listeningSocket(const ConnectionSetup & setup) noexcept {
    auto socket = ::socket(AF_INET, SOCK_STREAM, 0);
    auto address = getSocketAddress(setup);
    if (!address.has_value()) {
        close(socket);
        return ConnectionResponse{NET_ERROR::CRITICAL};
    }

    auto casted = reinterpret_cast<sockaddr *>(&address);
    int option = 1;
    if (setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) < 0)
        logError("setsockopt");

    if (bind(socket, casted, sizeof(address)) < 0) {
        logError("bind");
        close(socket);
        return ConnectionResponse{NET_ERROR::CRITICAL};
    }

    if (listen(socket, 32) < 0) {
        logError("listen");
        close(socket);
        return ConnectionResponse{NET_ERROR::CRITICAL};
    }

    logInfo(
        "Listening address: " + setup.address + ":" +
        std::to_string(setup.port));

    return ConnectionResponse{socket};
}

ConnectionResponse connectedSocket(const ConnectionSetup & setup) noexcept {
    auto socket = ::socket(AF_INET, SOCK_STREAM, 0);
    auto address = getSocketAddress(setup);
    if (!address.has_value()) {
        close(socket);
        return ConnectionResponse{NET_ERROR::CRITICAL};
    }

    auto casted = reinterpret_cast<sockaddr *>(&address);
    if (connect(socket, casted, sizeof(address)) < 0) {
        logError("connect");
        return ConnectionResponse{NET_ERROR::TEMPORARY};
    }

    logInfo(
        "Connected to address: " + setup.address + ":" +
        std::to_string(setup.port));

    return ConnectionResponse{socket};
}

IpPort getIpPort(int argc, char * argv[]) noexcept {
    auto envIp = getenv("CHAT_IP");
    auto envPort = getenv("CHAT_PORT");
    IpPort ipp;

    if (argc == 3) {
        ipp.ip = argv[1];
        ipp.port = std::stoi(argv[2]);
    } else if (envIp != nullptr && envPort != nullptr) {
        ipp.ip = envIp;
        ipp.port = std::stoi(envPort);
    } else {
        ipp.ip = "127.0.0.1";
        ipp.port = 50000;
    }
    return ipp;
}
