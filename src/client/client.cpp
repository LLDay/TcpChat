#include "client/client.h"
#include "ui_client.h"

#include "io_operations.h"
#include "utils.h"

#include <QDateTime>
#include <QRegularExpression>
#include <QTimer>

#include <unistd.h>

Client::Client(
    QStringView name,
    const EndpointSetup & setup,
    QWidget * parent) noexcept
    : QMainWindow{parent}, mName{name.toString()}, mSetup{setup.connection},
      mListener{setup.eventBufferSize, setup.timeout}, ui{new Ui::Client} {
    ui->setupUi(this);

    QObject::connect(
        ui->sendButton, &QPushButton::clicked, this, &Client::onSendClicked);

    QObject::connect(
        &mListener, &EventListenerAdapter::incomingMessage, this,
        &Client::onIncomingMessage);

    QObject::connect(
        &mListener, &EventListenerAdapter::connectionLost, this,
        &Client::onConnectionLost);

    ui->messageTextEdit->setFocus();
    connectToServer();
}

Client::~Client() noexcept {
    delete ui;
    ::close(mSocket);
}

void Client::onIncomingMessage() noexcept {
    auto callback = [this](const Message & message) {
        auto datetime = QDateTime::fromTime_t(message.datetime);
        auto messageString = datetime.toString("<HH:mm> ");
        messageString += "[" + QString::fromStdString(message.author) + "]\n";
        messageString += QString::fromStdString(message.text);

        ui->messagesListWidget->addItem(messageString);
        ui->messagesListWidget->scrollToBottom();
        ui->messageTextEdit->clear();
    };

    IoReadTask read{mSocket, callback};
    read.run();
    mListener.oneshot(mSocket);
}

void Client::onConnectionLost() noexcept {
    ::close(mSocket);
    logInfo("Disconnected from server");
    connectToServer();
}

void Client::onSendClicked() noexcept {
    static QRegularExpression middle{"[\\n]{2,}"};
    static QRegularExpression edge{"^\\n+|\\n+$"};

    auto text = ui->messageTextEdit->toPlainText().trimmed();

    if (text.isEmpty())
        return;

    text.replace(edge, "");
    text.replace(middle, "\n\n");

    Message message;
    message.author = mName.toStdString();
    message.text = text.toStdString();

    IoWriteTask writeTask{mSocket, message};
    writeTask.run();
}

void Client::connectToServer() noexcept {
    auto connectionResult = connectedSocket(mSetup);
    if (connectionResult.socket >= 0)
        mSocket = connectionResult.socket;

    if (connectionResult.error == NET_ERROR::TEMPORARY) {
        ui->statusLabel->setText("No server connection");
        ui->statusLabel->show();
        QTimer::singleShot(RECONNECT_TIME, this, &Client::connectToServer);
        return;
    }

    if (connectionResult.error == NET_ERROR::CRITICAL) {
        close();
        ui->statusLabel->setText("Wrong ip address or port");
        ui->statusLabel->show();
        return;
    }

    ui->statusLabel->hide();
    makeNonBlocking(mSocket);
    mListener.add(mSocket);
}
