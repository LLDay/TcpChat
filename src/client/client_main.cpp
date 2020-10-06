#include "client/client.h"

#include "client/name_dialog.h"
#include "setup.h"
#include "utils.h"

#include <QApplication>

#include <signal.h>

int main(int argc, char * argv[]) {
    signal(SIGPIPE, SIG_IGN);

    auto setupOptional = getSetup(argc, argv, "127.0.0.1", 50000);
    if (!setupOptional.has_value())
        return -1;

    auto setup = setupOptional.value();
    setup.eventBufferSize = 2;
    setup.timeout = 100;

    QApplication a{argc, argv};
    QString name;
    DialogName dialog;

    QObject::connect(
        &dialog, &DialogName::returnName,
        [&name](QString received) { name = received; });
    dialog.exec();

    if (name.isEmpty())
        return 0;

    Client w{name, setup};
    w.show();
    return a.exec();
}
