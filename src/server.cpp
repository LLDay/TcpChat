#include "server.h"

#include "chat_errors.h"

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <iostream>

Server::Server() {
    mSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (mSocket < 0)
        errorThrow("Socket");

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(SERVER_PORT);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if (bind(mSocket, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) < 0)
        errorThrow("Bind");
}

Server::~Server() noexcept {
    stop();
    if (mServerThread.joinable())
        mServerThread.join();
}

void Server::connectionFreed(ConnectionManager & manager) {
    std::lock_guard lock{mConnectionsMutex};
    auto index = &manager - mConnections.front().get();
    std::swap(mConnections[mDisconnectedNumber++], mConnections[index]);
}

void Server::onNewMessage(Message && message) noexcept {
    std::lock_guard lock{mMessagesMutex};
    mIncomingMessages.emplace_back(std::move(message));
}

void Server::join() {
    if (!mServerThread.joinable())
        throw std::runtime_error{"Thread is not joinable"};
    mServerThread.join();
}

void Server::start() {
    mServerThread = std::thread{&Server::work, this};
}

void Server::stop() noexcept {
    if (shutdown(mSocket, SHUT_RD) < 0)
        errorNoThrow("Shutdown");
    mFinish = true;
}

//
// [THREAD]
//
void Server::work() noexcept {
    std::cout << "Server started" << std::endl;

    if (listen(mSocket, BACKLOG) < 0)
        errorNoThrow("Listen");

    while (!mFinish) {
        auto socket = accept(mSocket, nullptr, nullptr);
        if (socket < 0)
            errorNoThrow("Accept");
        else
            newConnection(socket);
    }

    if (close(mSocket) < 0)
        errorNoThrow("Close");

    std::cout << "Server stopped" << std::endl;
}

void Server::flush() noexcept {
    std::unique_lock lockConnections{mConnectionsMutex, std::defer_lock};
    std::unique_lock lockMessages{mMessagesMutex, std::defer_lock};
    std::lock(lockConnections, lockMessages);

    auto messages = std::move(mIncomingMessages);
    mIncomingMessages = MessagesQueue(messages.size());
    lockMessages.unlock();

    for (auto & connection : mConnections)
        for (auto & message : messages)
            connection->send(message);
}

void Server::newConnection(int socket) noexcept {
    std::lock_guard lock{mConnectionsMutex};
    while (mDisconnectedNumber > 0 &&
           mConnections[mDisconnectedNumber - 1]->tryToConnect(socket))
        mDisconnectedNumber -= 1;

    if (mDisconnectedNumber == 0)
        mConnections.emplace_back(
            std::make_unique<ConnectionManager>(socket, *this));
}
