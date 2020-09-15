#pragma once

#include <string_view>

void errorThrow(std::string_view source);

void errorNoThrow(std::string_view source) noexcept;
