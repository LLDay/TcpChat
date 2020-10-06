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

    auto setupOptional = getSetup(argc, argv, "127.0.0.1", 50000);
    if (!setupOptional.has_value())
        return EXIT_FAILURE;

    auto setup = setupOptional.value();
    setup.eventBufferSize = 20;
    setup.timeout = 1000;

    server = std::make_unique<Server>(setup);
    server->start();
    server->join();

    return EXIT_SUCCESS;
}
