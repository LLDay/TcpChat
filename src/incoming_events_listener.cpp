#include "incoming_events_listener.h"

#include "utils.h"

#include <unistd.h>
#include <algorithm>
#include <iostream>
#include <stdexcept>

IncomingEventsListener::IncomingEventsListener(
    IncomingEventHandler & handler,
    int eventBufferSize,
    int timeout)
    : mHandler{handler}, mTimeout{timeout}, mBufferSize{eventBufferSize} {
    if (mBufferSize < 0)
        throw std::invalid_argument{"No buffer memory"};

    mEventBuffer = std::make_unique<epoll_event[]>(mBufferSize);
    mEpoll = epoll_create(mBufferSize);
    if (mEpoll < 0) {
        logError("epoll_create");
        stop();
    }
}

void IncomingEventsListener::add(int socket) noexcept {
    auto event = epoll_event{};
    event.data.fd = socket;
    event.events = EPOLLIN | EPOLLONESHOT | EPOLLRDHUP | EPOLLERR;

    if (epoll_ctl(mEpoll, EPOLL_CTL_ADD, socket, &event) < 0)
        logError("epoll_ctl_add");
}

void IncomingEventsListener::oneshot(int socket) noexcept {
    auto event = epoll_event{};
    event.data.fd = socket;
    event.events = EPOLLIN | EPOLLONESHOT | EPOLLRDHUP | EPOLLERR;

    if (epoll_ctl(mEpoll, EPOLL_CTL_MOD, socket, &event) < 0)
        logError("epoll_ctl_add");
}

void IncomingEventsListener::threadStep() noexcept {
    auto socketsNumber =
        epoll_wait(mEpoll, mEventBuffer.get(), mBufferSize, mTimeout);

    for (auto i = 0; i < socketsNumber; ++i) {
        auto eventType = mEventBuffer[i].events;
        auto socket = mEventBuffer[i].data.fd;

        if (eventType & (EPOLLRDHUP | EPOLLERR))
            mHandler.onConnectionLost(socket);
        else if (eventType & EPOLLIN)
            mHandler.onIncomingMessageFrom(socket);
    }
}

void IncomingEventsListener::onThreadFinish() noexcept {
    if (close(mEpoll) < 0)
        logError("close");
}
