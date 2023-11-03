//
// Created by eynnzerr on 23-11-1.
//

#include "StreamConfigItem.h"

StreamConfigItem::StreamConfigItem(int index) {
    this->index = index;
    setupUI();
    initSignalSlots();
}

void StreamConfigItem::setupUI() {
    auto hLayout = new QHBoxLayout;
    auto indexLabel = new QLabel(QString::number(index));

    typeBox = new QComboBox;
    QStringList typeOptions;
    typeOptions << "水声通信" << "wifi通信" << "光通信";
    typeBox->addItems(typeOptions);
    typeBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    streamRate = new QLineEdit;

    auto nodeLabel = new QLabel("node:");
    auto toLabel = new QLabel("to");
    auto timeLabel = new QLabel("time");
    auto toLabel2 = new QLabel("to");

    auto intValidator = new QIntValidator;
    intValidator->setBottom(0); // TODO 如何获取当前已创建流数量以限制最大值？Qt是否具有全局通信机制？
    senderNode = new QLineEdit;
    senderNode->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    senderNode->setValidator(intValidator);
    receiverNode = new QLineEdit;
    receiverNode->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    receiverNode->setValidator(intValidator);

    startTime = new QLineEdit;
    startTime->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    startTime->setValidator(intValidator);
    endTime = new QLineEdit;
    endTime->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    endTime->setValidator(intValidator);

    hLayout->addWidget(indexLabel);
    hLayout->addWidget(typeBox);
    hLayout->addWidget(streamRate);
    hLayout->addWidget(nodeLabel);
    hLayout->addWidget(senderNode);
    hLayout->addWidget(toLabel);
    hLayout->addWidget(receiverNode);
    hLayout->addWidget(timeLabel);
    hLayout->addWidget(startTime);
    hLayout->addWidget(toLabel2);
    hLayout->addWidget(endTime);

    setLayout(hLayout);
}

void StreamConfigItem::initSignalSlots() {

}

int StreamConfigItem::getType() {
    return typeBox->currentIndex();
}

int StreamConfigItem::getSenderNode() {
    return senderNode->text().toInt();
}

int StreamConfigItem::getReceiverNode() {
    return receiverNode->text().toInt();
}

double StreamConfigItem::getRate() {
    return streamRate->text().toDouble();
}

int StreamConfigItem::getStartTime() {
    return startTime->text().toInt();
}

int StreamConfigItem::getEndTime() {
    return startTime->text().toInt();
}

QJsonObject StreamConfigItem::toJsonObject() {
    QJsonObject jsonObject;
    jsonObject["type"] = getType();
    jsonObject["sender"] = getSenderNode();
    jsonObject["receiver"] = getReceiverNode();
    jsonObject["rate"] = getRate();
    jsonObject["start_time"] = getStartTime();
    jsonObject["end_time"] = getEndTime();
    return jsonObject;
}
