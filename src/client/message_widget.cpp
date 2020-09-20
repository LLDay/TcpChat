#include "client/message_widget.h"
#include "ui_message_widget.h"

#include "message.h"

#include <QDateTime>
#include <QString>
#include <QDebug>

MessageWidget::MessageWidget(const Message & message, QWidget * parent)
    : QWidget{parent}, ui{new Ui::MessageWidget} {
    ui->setupUi(this);

    auto time = QDateTime::fromTime_t(message.datetime).toLocalTime();

    ui->textLabel->setWordWrap(true);
    ui->textLabel->setText(QString::fromStdString(message.text));
    ui->authorLabel->setText(QString::fromStdString(message.author));
    ui->dateLabel->setText(time.toString("hh.mm"));
}

MessageWidget::~MessageWidget() {
    delete ui;
}
