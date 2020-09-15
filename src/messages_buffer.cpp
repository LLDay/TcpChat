#include "messages_buffer.h"

MessagesBuffer::MessagesBuffer(size_t initialSize) noexcept {
    mContainer.resize(initialSize);
}

template <class Container, class MessageType>
inline void emplace(Container & container, MessageType && message) {
    container.emplace_back(std::forward<MessageType>(message));
}

void MessagesBuffer::push(const Message & message) noexcept {
    emplace(mContainer, message);
}

void MessagesBuffer::push(Message && message) noexcept {
    emplace(mContainer, std::move(message));
}

MessagesBuffer::Container MessagesBuffer::flush() noexcept {
    std::unique_lock lock{mMutex};
    auto tmpContainer = std::move(mContainer);
    mContainer = MessagesBuffer::Container{};
    mContainer.resize(tmpContainer.size() / 2);
    lock.unlock();

    return tmpContainer;
}

bool MessagesBuffer::isEmpty() const noexcept {
    std::lock_guard lock{mMutex};
    return mContainer.empty();
}
