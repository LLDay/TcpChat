#include "connection_listener.h"

#include "server_log.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>

ConnectionListener::ConnectionListener(
    IConnectionHandler & handler,
    const ConnectionSetup & setup) noexcept
    : mHandler{handler}, mSetup{setup}, mSocket{-1} {}

void ConnectionListener::onStop() noexcept {
    if (shutdown(mSocket, SHUT_RD))
        serverError("shutdown");
}

void ConnectionListener::onThreadStart() noexcept {
    mSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (mSocket < 0) {
        serverError("Socket");
        stop();
        return;
    }

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(mSetup.port);

    if (inet_pton(AF_INET, mSetup.address.c_str(), &addr.sin_addr) <= 0)
        serverError("inet_pton");

    if (bind(mSocket, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) < 0) {
        serverError("bind");
        stop();
    }

    if (listen(mSocket, BACKLOG) < 0) {
        serverError("listen");
        stop();
    }
}

void ConnectionListener::threadStep() noexcept {
    auto socket = accept(mSocket, nullptr, nullptr);
    if (isStopped())
        return;

    if (socket < 0) {
        serverError("Accept");
    }
    mHandler.onNewConnection(socket);
}

void ConnectionListener::onThreadFinish() noexcept {
    if (close(mSocket) < 0)
        serverError("Close");
}
