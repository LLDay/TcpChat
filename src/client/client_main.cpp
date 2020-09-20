#include "client/client.h"

#include "setup.h"
#include "client/dialog_name.h"

#include <QApplication>
#include <QString>

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

    ConnectionSetup setup;
    setup.address = "127.0.0.1";
    setup.port = 50000;

    Client w{name, setup};
    w.show();
    return a.exec();
}
