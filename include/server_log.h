#pragma once

#include <string_view>

void serverError(std::string_view source) noexcept;

void serverInfo(std::string_view info) noexcept;
