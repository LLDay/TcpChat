#include "client/client.h"

#include "setup.h"
#include "client/dialog_name.h"

#include <QApplication>

int main(int argc, char * argv[]) {
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
    setup.connection.address = "127.0.0.1";
    setup.connection.port = 50000;
    setup.eventBufferSize = 2;
    setup.timeout = 100;

    Client w{name, setup};
    w.show();
    return a.exec();
}
