#include "connection.h"

#include <cerrno>
#include <cstring>
#include <iostream>

#include <unistd.h>

ConnectionManager::ConnectionManager(int socket, Server & server) noexcept
    : mSocket{socket}, mServer{server} {}

ConnectionManager::~ConnectionManager() noexcept {
    mFinished = true;
    mConditional.notify_all();
    mSessionThread.join();
}

void ConnectionManager::finish() noexcept {
    mFinished = true;
}

void ConnectionManager::send(const Message & message) noexcept {
    {
        std::lock_guard lock{mMutex};
        mQueue.emplace_back(message);
    }
    mConditional.notify_all();
}

void ConnectionManager::send(Message && message) noexcept {
    {
        std::lock_guard lock{mMutex};
        mQueue.emplace_back(std::move(message));
    }
    mConditional.notify_all();
}

bool ConnectionManager::tryToConnect(int newSocket) noexcept {
    if (!mDisconnected)
        return false;

    std::lock_guard lock{mMutex};
    mSocket = newSocket;

    return true;
}

//
// [THREAD]
//
void ConnectionManager::session() noexcept {
    while (true) {
        std::unique_lock lock{mMutex};
        mConditional.wait(
            lock, [this]() { return !mQueue.empty() || mFinished; });

        if (mFinished)
            break;

        auto queue = std::move(mQueue);
        mQueue = MessagesQueue(queue.size());

        lock.unlock();

        for (auto & message : queue) {
            // write
        }
    }

    if (close(mSocket))
        std::cerr << strerror(errno) << std::endl;
}
