#pragma once

#include "message.h"

#include <mutex>
#include <vector>

class MessagesBuffer {
public:
    using Container = std::vector<Message>;

public:
    explicit MessagesBuffer(size_t initialSize = 0) noexcept;

    void push(const Message & message) noexcept;

    void push(Message && message) noexcept;

    [[nodiscard]]
    Container flush() noexcept;

    bool isEmpty() const noexcept;

private:
    mutable std::mutex mMutex;
    Container mContainer;
};
