#include "client/client.h"
#include <arpa/inet.h>
#include <unistd.h>
#include "utils.h"
#include "ui_client.h"

#include "client/dialog_name.h"
#include "client/message_widget.h"
#include "io_operations.h"
#include "setup.h"

#include <QDebug>
#include <QThread>
#include <algorithm>

#include <sys/socket.h>

Client::Client(
    QStringView name,
    const ConnectionSetup & setup,
    QWidget * parent) noexcept
    : QMainWindow{parent}, mName{name.toString()}, mListener{1, 200},
      ui{new Ui::Client} {
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

    mSocket = connectedSocket(setup);
    makeNonBlocking(mSocket);
    mListener.add(mSocket);
}

Client::~Client() noexcept {
    delete ui;
}

void Client::onSendClicked() noexcept {
    auto text = ui->messageTextEdit->toPlainText().toStdString();

    if (text.empty())
        return;

    Message message;
    message.author = mName.toStdString();
    message.text = std::move(text);

    auto rawMessage = message.serialize();

    if (write(mSocket, rawMessage.data(), rawMessage.size()) < 0)
        logError("write");
}

void Client::onIncomingMessage() noexcept {
    auto callback = [this](const Message & message) {
        qDebug() << "Received " << message.text.size() << " bytes";

        auto item = new QListWidgetItem{ui->messagesListWidget};
        auto messageWidget = new MessageWidget{message, ui->messagesListWidget};

        item->setSizeHint(messageWidget->sizeHint());
        ui->messagesListWidget->addItem(item);
        ui->messagesListWidget->setItemWidget(item, messageWidget);
        ui->messageTextEdit->clear();
    };

    IoReadTask read{mSocket, callback};
    read.run();
    mListener.oneshot(mSocket);
}

void Client::onConnectionLost() noexcept {}
