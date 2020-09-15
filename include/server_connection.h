#pragma once

#include "interuptable.h"
#include "messages_buffer.h"

#include <mutex>
#include <atomic>
#include <string>

class Server;

class ServerConnection : public Interuptable {
public:
    constexpr static size_t MAX_NAME_LENGTH = 64;

public:
    explicit ServerConnection(int socket, Server & server) noexcept;

    ~ServerConnection() noexcept;

    void connectToNewClient(int newSocket);

private:
    void step() noexcept override;

    void onThreadStart() noexcept override;

    void onThreadFinish() noexcept override;

    bool setupClientConnection() noexcept;

private:
    int mSocket;
    Server & mServer;

    std::mutex mMutex;
    std::string mName;
    std::atomic_bool mIsDisconnected = true;

    MessagesBuffer mBuffer;
};

