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

    auto setupOptinal = getSetup(argc, argv);
    if (!setupOptinal.has_value())
        return EXIT_FAILURE;

    auto setup = setupOptinal.value();
    setup.eventBufferSize = 20;
    setup.timeout = 1000;

    server = std::make_unique<Server>(setup);
    server->start();
    server->join();

    return EXIT_SUCCESS;
}
