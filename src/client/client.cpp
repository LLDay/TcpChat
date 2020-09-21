#include "client/client.h"

#include "client/dialog_name.h"
#include "client/message_widget.h"

#include "io_operations.h"
#include "setup.h"
#include "ui_client.h"
#include "utils.h"

#include <unistd.h>

#include <QDebug>

Client::Client(
    QStringView name,
    const EndpointSetup & setup,
    QWidget * parent) noexcept
    : QMainWindow{parent}, mName{name.toString()},
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

    mSocket = connectedSocket(setup.connection);
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

    IoWriteTask writeTask{mSocket, message};
    writeTask.run();
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
