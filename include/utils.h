#pragma once

#include <string_view>

struct ConnectionSetup;

void logError(std::string_view source) noexcept;

void logInfo(std::string_view info) noexcept;

void makeNonBlocking(int fd) noexcept;

int bindedSocket(const ConnectionSetup & setup) noexcept;

int connectedSocket(const ConnectionSetup & setup) noexcept;
