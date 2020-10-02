#include "client/client.h"

#include "client/name_dialog.h"
#include "setup.h"

#include <QApplication>

#include <signal.h>

int main(int argc, char * argv[]) {
    signal(SIGPIPE, SIG_IGN);

    QApplication a{argc, argv};
    QString name;
    DialogName dialog;

    QObject::connect(
        &dialog, &DialogName::returnName,
        [&name](QString received) { name = received; });
    dialog.exec();

    if (name.isEmpty())
        return 0;

    EndpointSetup setup;
    if (argc == 3) {
        setup.connection.address = argv[1];
        setup.connection.port = std::stoi(argv[2]);
    } else {
        setup.connection.address = "127.0.0.1";
        setup.connection.port = 50000;
    }

    setup.eventBufferSize = 2;
    setup.timeout = 100;

    Client w{name, setup};
    w.show();
    return a.exec();
}
