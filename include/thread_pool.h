#pragma once

#include "message.h"
#include "workers.h"

#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>

class MessagesBuffer;

class IThreadPool {
    public:
        virtual ~IThreadPool() noexcept;

        int getSocketToWork();

    protected:
        virtual void asyncAccess() noexcept;

        void notify() noexcept;

        void notifyAll() noexcept;

    private:
        std::mutex mCommonMutex;
        std::condition_variable mConditional;
};

class ThreadPool : private IThreadPool {
public:
    using SocketList = std::vector<int>;

public:
    ThreadPool(size_t size) noexcept;

    ~ThreadPool();

    void receive(int socket) noexcept;

    void broadcast(SocketList && sockets, std::vector<Message> messages);

    void resize(size_t newSize);

private:
    void asyncAccess() noexcept override;

private:
    SocketList mReceiveList;
    SocketList mSendList;
    std::mutex mListLock;
};

