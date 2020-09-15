#pragma once

#include <atomic>
#include <thread>

class Message;
class IThreadPool;
class MessagesBuffer;

class MessageTransceiver {
public:
    MessageTransceiver(
        IThreadPool & threadPool,
        MessagesBuffer & buffer) noexcept;

    ~MessageTransceiver() noexcept;

    void stop() noexcept;

private:
    void run() noexcept;

    void send(int socket, Message && message) const noexcept;

    void receive(int socket) const noexcept;

private:
    IThreadPool & mThreadPool;
    MessagesBuffer & mBuffer;

    std::thread mThread;
    std::atomic_bool mActive = true;
};
