#include "server_log.h"

#include <cerrno>
#include <cstring>
#include <iostream>
#include <mutex>

void serverError(std::string_view source) noexcept {
    static std::mutex lMutex;
    std::lock_guard {lMutex};

    std::cerr << "ERROR: <" << source << "> " << strerror(errno) << std::endl;
}

void serverInfo(std::string_view info) noexcept {
    static std::mutex lMutex;
    std::lock_guard {lMutex};

    std::cout << "INFO: " << info << std::endl;
}
