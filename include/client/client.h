#pragma once

#include "event_listener_adapter.h"
#include "setup.h"

#include <QMainWindow>
#include <QString>

struct Message;

namespace Ui {
class Client;
}

class Client : public QMainWindow {
    Q_OBJECT

private:
    static constexpr int RECONNECT_TIME = 5000;

public:
    explicit Client(
        QStringView name,
        const EndpointSetup & setup,
        QWidget * parent = nullptr) noexcept;

    ~Client() noexcept;

public slots:
    void onIncomingMessage() noexcept;

    void onConnectionLost() noexcept;

    void onSendClicked() noexcept;

private slots:
    void connectToServer() noexcept;

private:
    int mSocket;
    QString mName;
    ConnectionSetup mSetup;

    EventListenerAdapter mListener;
    Ui::Client * ui;
};
