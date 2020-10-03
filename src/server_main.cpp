#include "server.h"

#include "setup.h"
#include "utils.h"

#include <signal.h>

#include <memory>

std::unique_ptr<Server> server;

void signalHandler(int signal) {
    if (signal == SIGINT)
        server->stop();
}

int main(int argc, char * argv[]) {
    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, signalHandler);

    EndpointSetup setup;
    setup.parallelWorkers = 4;
    setup.eventBufferSize = 20;
    setup.timeout = 1000;

    auto ipp = getIpPort(argc, argv);
    setup.connection.address = ipp.ip;
    setup.connection.port = ipp.port;

    server = std::make_unique<Server>(setup);
    server->start();
    server->join();

    return EXIT_SUCCESS;
}
