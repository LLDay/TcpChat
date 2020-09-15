#include "chat_errors.h"

#include <cerrno>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>

void errorThrow(std::string_view source) {
    std::string errorSource{source};
    throw std::runtime_error{errorSource + ": " + strerror(errno)};
}

void errorNoThrow(std::string_view source) noexcept {
    std::cerr << source << ": " << strerror(errno) << std::endl;
}
