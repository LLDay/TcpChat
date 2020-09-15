#include "server_connection.h"

#include "chat_errors.h"

#include <sys/epoll.h>
#include <unistd.h>

#include <iostream>

ServerConnection::ServerConnection(int socket, Server & server) noexcept
    : mSocket{socket}, mServer{server} {}

ServerConnection::~ServerConnection() noexcept {}

void ServerConnection::connectToNewClient(int newSocket) {
    std::lock_guard lock{mMutex};
    mSocket = newSocket;
}

void ServerConnection::step() noexcept {
    if (isStopped())
        return;

    if (mIsDisconnected)
        setupClientConnection();

    const auto epollFd = epoll_create(1);

    std::unique_lock lock{mMutex};
    const auto messages = mBuffer.flush();
}

void ServerConnection::onThreadStart() noexcept {}

void ServerConnection::onThreadFinish() noexcept {
    close(mSocket);
}

bool ServerConnection::setupClientConnection() noexcept {
    std::lock_guard lock{mMutex};
    mName.clear();

    char buf[MAX_NAME_LENGTH];
    auto numRead = read(mSocket, buf, MAX_NAME_LENGTH);

    if (numRead < 0) {
        errorNoThrow("Read");
        return false;
    }

    if (numRead == 0)
        return false;

    mName.append(buf);
    mIsDisconnected = false;

    return true;
}
