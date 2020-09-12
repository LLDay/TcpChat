#pragma once

#include "message.h"

#include <atomic>
#include <condition_variable>
#include <thread>
#include <vector>

class Server;

class ConnectionManager {
public:
    using MessagesQueue = std::vector<Message>;

public:
    explicit ConnectionManager(int socket, Server & server) noexcept;

    ~ConnectionManager() noexcept;

    void finish() noexcept;

    void send(const Message & message) noexcept;

    void send(Message && message) noexcept;

    bool tryToConnect(int newSocket) noexcept;

    bool isConnected() noexcept;

private:
    void session() noexcept;

private:
    int mSocket;
    Server & mServer;

    std::atomic_bool mFinished = false;
    std::atomic_bool mDisconnected = true;

    MessagesQueue mQueue;
    std::condition_variable mConditional;
    std::mutex mMutex;
    std::thread mSessionThread;
};

