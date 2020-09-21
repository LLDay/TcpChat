#pragma once

#include "event_listener_adapter.h"

#include <QMainWindow>
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

    void onConnectionLost() noexcept;

    void onSendClicked() noexcept;

private:
    qint32 mSocket;
    QString mName;
    EventListenerAdapter mListener;
    Ui::Client * ui;
};
