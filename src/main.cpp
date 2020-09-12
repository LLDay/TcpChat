#include "server.h"

#include <signal.h>
#include <unistd.h>

#include <iostream>
#include <vector>

Server server;

void signalHandler(int signal) {
    if (signal == SIGINT)
        server.stop();
}

int main(int argc, char ** argv) {
    signal(SIGINT, signalHandler);
    signal(SIGPIPE, SIG_IGN);
    server.start();
    server.join();
    return EXIT_SUCCESS;
}
