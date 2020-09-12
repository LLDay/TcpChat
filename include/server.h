#pragma once

#include "connection.h"
#include "message.h"

#include <memory.h>
#include <atomic>
#include <mutex>
#include <thread>
#include <vector>

class Server {
public:
    // Use unique_ptr to move items within a vector for optimisation
    using ConnectionsPool = std::vector<std::unique_ptr<ConnectionManager>>;
    using MessagesQueue = ConnectionManager::MessagesQueue;

public:
    static constexpr size_t SERVER_PORT = 30852;
    static constexpr size_t BACKLOG = 32;

public:
    Server();

    Server(Server &) = delete;

    Server & operator=(Server &) = delete;

    ~Server() noexcept;

    void connectionFreed(ConnectionManager & manager);

    void onNewMessage(Message && message) noexcept;

    void join();

    void start();

    void stop() noexcept;

private:
    void work() noexcept;

    void flush() noexcept;

    void newConnection(int socket) noexcept;

private:
    int mSocket;

    std::atomic_bool mFinish = false;
    std::thread mServerThread;

    std::mutex mMessagesMutex;
    MessagesQueue mIncomingMessages;

    size_t mDisconnectedNumber;
    std::mutex mConnectionsMutex;
    ConnectionsPool mConnections;
};
