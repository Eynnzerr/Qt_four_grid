//
// Created by eynnzerr on 23-10-31.
//

#include "NodeConfigItem.h"

NodeConfigItem::NodeConfigItem(int index) {
    this->index = index;
    setupUI();
    initSignalSlots();
}

void NodeConfigItem::setupUI() {
    auto hLayout = new QHBoxLayout;
    auto indexLabel = new QLabel(QString::number(index));

    nodeTypeBox = new QComboBox;
    QStringList typeOptions;
    typeOptions << "无人机" << "无人船" << "水声设备";
    nodeTypeBox->addItems(typeOptions);
    nodeTypeBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    moveMode = new QComboBox;
    QStringList modeOptions;
    modeOptions << "静止" << "随机扰动" << "固定速度";
    moveMode->addItems(modeOptions);
    moveMode->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    QRegExp regex("^[0-9]+(\\.[0-9]+)?$");  // 正则表达式，用于匹配正数（包括整数和小数）
    QRegExpValidator validator(regex);

    // TODO 非静止时才可编辑速度
    moveVelocity = new QLineEdit;
    moveVelocity->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    moveVelocity->setValidator(&validator);

    positionX = new QLineEdit;
    positionX->setFixedWidth(50);
    positionX->setStyleSheet("border: none; border-bottom: 1px solid black;");
    positionX->setValidator(&validator);

    positionY = new QLineEdit;
    positionY->setFixedWidth(50);
    positionY->setStyleSheet("border: none; border-bottom: 1px solid black;");
    positionY->setValidator(&validator);

    positionZ = new QLineEdit;
    positionZ->setFixedWidth(50);
    positionZ->setStyleSheet("border: none; border-bottom: 1px solid black;");
    positionZ->setValidator(&validator);

    hLayout->addWidget(indexLabel);
    hLayout->addWidget(nodeTypeBox);
    hLayout->addWidget(moveMode);
    hLayout->addWidget(moveVelocity);
    hLayout->addWidget(positionX);
    hLayout->addWidget(positionY);
    hLayout->addWidget(positionZ);
    setLayout(hLayout);
}

void NodeConfigItem::initSignalSlots() {

}

int NodeConfigItem::getNodeType() {
    return nodeTypeBox->currentIndex();
}

int NodeConfigItem::getMoveMode() {
    return moveMode->currentIndex();
}

double NodeConfigItem::getVelocity() {
    return moveVelocity->text().toDouble();
}

double NodeConfigItem::getPositionX() {
    return positionX->text().toDouble();
}

double NodeConfigItem::getPositionY() {
    return positionY->text().toDouble();
}

double NodeConfigItem::getPositionZ() {
    return positionZ->text().toDouble();
}

QJsonObject NodeConfigItem::toJsonObject() {
    QJsonObject jsonObject;
    jsonObject["type"] = getNodeType();
    jsonObject["move_mode"] = getMoveMode();
    jsonObject["velocity"] = getVelocity();
    jsonObject["position_x"] = getPositionX();
    jsonObject["position_y"] = getPositionY();
    jsonObject["position_z"] = getPositionZ();
    return jsonObject;
}
