#include "message.h"

#include <algorithm>
#include <cstring>
#include <iostream>

std::string Message::serialize() const noexcept {
    constexpr auto timeSize = sizeof(TimeType);
    char portableTime[timeSize];

    for (size_t i = 0; i < timeSize; ++i)
        portableTime[i] = (datetime >> (8 * i)) & 0xFF;

    std::string serialized;
    serialized.reserve(timeSize + author.size() + text.size() + 3);
    serialized.append(portableTime, timeSize);
    serialized.append("#");
    serialized.append(author);
    serialized.append("#");
    serialized.append(text);
    serialized.append("\0");

    return serialized;
}

Message Message::deserialize(const char * data, size_t size) noexcept {
    constexpr auto timeSize = sizeof(TimeType);

    Message deserialized;

    auto dataEnd = data + size;
    auto authorStart = data + timeSize + 1;
    auto authorEnd = std::find(authorStart, dataEnd, '#');

    memcpy(&deserialized.datetime, data, timeSize);
    std::copy(authorStart, authorEnd, std::back_inserter(deserialized.author));
    std::copy(authorEnd + 1, dataEnd, std::back_inserter(deserialized.text));

    return deserialized;
}
