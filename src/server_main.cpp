#include "server.h"

#include "setup.h"

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

    if (argc == 3) {
        setup.connection.address = argv[1];
        setup.connection.port = std::stoi(argv[2]);
    } else {
        setup.connection.address = "127.0.0.1";
        setup.connection.port = 50000;
    }

    server = std::make_unique<Server>(setup);
    server->start();
    server->join();

    return EXIT_SUCCESS;
}
