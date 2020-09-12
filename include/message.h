#pragma once

#include <string>
#include <time.h>

struct Message {
    time_t time;
    std::string author;
    std::string message;
};

