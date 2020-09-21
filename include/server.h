#pragma once

#include "connection_listener.h"
#include "incoming_events_listener.h"
#include "manual_control.h"
#include "message.h"
#include "workers_pool.h"

#include <mutex>

struct EndpointSetup;

class Server : public IConnectionHandler,
               public IncomingEventHandler,
               public ManualControl {
public:
    using SocketList = std::vector<int>;

public:
    Server(const EndpointSetup & setup) noexcept;

    ~Server() noexcept;

    void join() noexcept;

private:
    void onStart() noexcept override;

    void onStop() noexcept override;

    void onNewConnection(int socket) noexcept override;

    void onIncomingMessageFrom(int socket) noexcept override;

    void onConnectionLost(int socket) noexcept override;

    void onMessageReceived(int socket, const Message & message) noexcept;

private:
    std::mutex mMutex;
    SocketList mSockets;

    ConnectionListener mListener;
    IncomingEventsListener mIncomingEventsListener;
    WorkersPool mWorkers;
};
