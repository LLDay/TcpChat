#include "client/client.h"

#include "client/dialog_name.h"
#include "client/message_widget.h"

#include "io_operations.h"
#include "ui_client.h"
#include "utils.h"

#include <unistd.h>

#include <QRegularExpression>
#include <QTimer>

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

void Client::onConnectionLost() noexcept {
    ::close(mSocket);
    connectToServer();
}

void Client::onSendClicked() noexcept {
    static QRegularExpression middle{"[\\n]{2,}"};
    static QRegularExpression edge{"^\\n+|\\n+$"};

    auto text = ui->messageTextEdit->toPlainText();

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
    mSocket = connectedSocket(mSetup);

    if (mSocket < 0) {
        ui->statusLabel->show();
        QTimer::singleShot(RECONNECT_TIME, this, &Client::connectToServer);
        return;
    }

    ui->statusLabel->hide();
    makeNonBlocking(mSocket);
    mListener.add(mSocket);
}
