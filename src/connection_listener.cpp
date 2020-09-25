#include "connection_listener.h"

#include "setup.h"
#include "utils.h"

#include <unistd.h>

ConnectionListener::ConnectionListener(
    IConnectionHandler & handler,
    const ConnectionSetup & setup) noexcept
    : mHandler{handler} {
    mSocket = listeningSocket(setup);
    if (mSocket < 0)
        stop();
}

ConnectionListener::~ConnectionListener() noexcept {
    MANUAL_FINISH
}

void ConnectionListener::onStop() noexcept {
    if (shutdown(mSocket, SHUT_RD))
        logError("shutdown");
}

void ConnectionListener::threadStep() noexcept {
    auto socket = accept(mSocket, nullptr, nullptr);
    if (isStopped())
        return;

    if (socket < 0) {
        logError("Accept");
    }
    mHandler.onNewConnection(socket);
}

void ConnectionListener::onThreadFinish() noexcept {
    if (close(mSocket) < 0)
        logError("Close");
}
