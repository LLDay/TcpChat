#pragma once

#include "incoming_events_listener.h"
#include "manual_control.h"
#include "workers_pool.h"

#include <QMainWindow>
#include <QTcpSocket>
#include <QString>

struct ConnectionSetup;
struct Message;

namespace Ui {
class Client;
}

class Client : public QMainWindow {
    Q_OBJECT
public:
    explicit Client(
        QStringView name,
        const ConnectionSetup & setup,
        QWidget * parent = nullptr) noexcept;

    ~Client() noexcept;

public slots:
    void onIncomingMessage() noexcept;

    void onSendClicked() noexcept;

private:
    QTcpSocket mSocket;
    QString mName;
    Ui::Client * ui;
};
