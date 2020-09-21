#include "server.h"

#include "runnable.h"
#include "setup.h"

#include <signal.h>
#include <time.h>
#include <unistd.h>

#include <iostream>
#include <iomanip>
#include <memory>
#include <vector>
#include <chrono>

std::unique_ptr<Server> server;

void signalHandler(int signal) {
    if (signal == SIGINT)
        server->stop();
}

int main() {
    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, signalHandler);

    EndpointSetup setup;
    setup.parallelWorkers = 4;
    setup.eventBufferSize = 20;
    setup.timeout = 1000;
    setup.connection.port = 50000;
    setup.connection.address = "127.0.0.1";

    server = std::make_unique<Server>(setup);
    server->start();
    server->join();

    return EXIT_SUCCESS;
}
