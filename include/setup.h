#pragma once

#include <netinet/in.h>
#include <string>
#include <thread>

struct ConnectionSetup {
    in_port_t port;
    std::string address;
};

struct EndpointSetup {
    ConnectionSetup connection;
    int parallelWorkers;
    int eventBufferSize;
    int timeout;
};
