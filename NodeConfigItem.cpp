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
    moveMode = new QComboBox;
    QStringList modeOptions;
    modeOptions << "静止" << "随机扰动" << "固定路线";
    moveMode->addItems(modeOptions);
    moveMode->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    moveVelocity = new QLineEdit;
    moveVelocity->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    hLayout->addWidget(indexLabel);
    hLayout->addWidget(moveMode);
    hLayout->addWidget(moveVelocity);
    setLayout(hLayout);
}

void NodeConfigItem::initSignalSlots() {

}
