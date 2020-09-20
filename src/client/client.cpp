#include "client/client.h"
#include <unistd.h>
#include "ui_client.h"

#include "client/dialog_name.h"
#include "client/message_widget.h"
#include "io_operations.h"
#include "setup.h"

#include <QDebug>
#include <QThread>
#include <algorithm>

Client::Client(
    QStringView name,
    const ConnectionSetup & setup,
    QWidget * parent) noexcept
    : QMainWindow{parent}, mName{name.toString()}, ui{new Ui::Client} {
    ui->setupUi(this);

    QObject::connect(
        ui->sendButton, &QPushButton::clicked, this, &Client::onSendClicked);

    QObject::connect(
        &mSocket, &QTcpSocket::readyRead, this, &Client::onIncomingMessage);

    ui->messageTextEdit->setFocus();

    mSocket.connectToHost(
        QString::fromStdString(setup.address), setup.port,
        QAbstractSocket::ReadWrite,
        QAbstractSocket::NetworkLayerProtocol::IPv4Protocol);
}

Client::~Client() noexcept {
    delete ui;
}

void Client::onIncomingMessage() noexcept {
    QByteArray array;

    while (mSocket.bytesAvailable())
        array += mSocket.readLine();

    qDebug() << "Received " << array.size() << " bytes";

    auto message = Message::deserialize(array.data(), array.size());
    auto item = new QListWidgetItem{ui->messagesListWidget};
    auto messageWidget = new MessageWidget{message, this};

    item->setSizeHint(messageWidget->sizeHint());
    ui->messagesListWidget->addItem(item);
    ui->messagesListWidget->setItemWidget(item, messageWidget);
    ui->messageTextEdit->clear();
}

void Client::onSendClicked() noexcept {
    auto text = ui->messageTextEdit->toPlainText().toStdString();

    if (text.empty())
        return;

    Message message;
    message.author = mName.toStdString();
    message.text = std::move(text);

    auto rawMessage = message.serialize();
    mSocket.write(rawMessage.data(), rawMessage.size());
}
