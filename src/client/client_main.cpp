#include "client/client.h"

#include "client/name_dialog.h"
#include "setup.h"
#include "utils.h"

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

    auto ipp = getIpPort(argc, argv);
    setup.connection.address = ipp.ip;
    setup.connection.port = ipp.port;

    setup.eventBufferSize = 2;
    setup.timeout = 100;

    Client w{name, setup};
    w.show();
    return a.exec();
}
