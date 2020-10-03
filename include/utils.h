#pragma once

#include <string>

struct ConnectionSetup;

enum class NET_ERROR : int { TEMPORARY, CRITICAL, NONE };

struct ConnectionResponse {
    explicit ConnectionResponse(int s) noexcept
        : socket{s}, error{NET_ERROR::NONE} {}

    explicit ConnectionResponse(NET_ERROR e) noexcept : socket{-1}, error{e} {}

    int socket;
    NET_ERROR error;
};

struct IpPort {
    std::string ip;
    int port;
};

void logError(std::string_view source) noexcept;

void logInfo(std::string_view info) noexcept;

void makeNonBlocking(int fd) noexcept;

ConnectionResponse listeningSocket(const ConnectionSetup & setup) noexcept;

ConnectionResponse connectedSocket(const ConnectionSetup & setup) noexcept;

IpPort getIpPort(int argc, char * argv[]) noexcept;
