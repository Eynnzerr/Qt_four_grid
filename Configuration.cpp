//
// Created by eynnzerr on 23-10-30.
//

#include "Configuration.h"
#include <QScrollArea>

Configuration::Configuration(QWidget *parent): QWidget(parent)
{
    setupUI();
    initSignalSlots();
}

Configuration::~Configuration() = default;

void Configuration::setupUI() {
    auto formLayout = new QFormLayout;
    formLayout->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);

    // node params
    auto nodeConfigTitle = new QLabel("Node Params");
    nodeConfigTitle->setStyleSheet("QLabel {font-weight: bold; font-size: 20px}");
    formLayout->addRow(nodeConfigTitle);

    auto uavNodeLabel = new QLabel("1. UAV");
    uavNodeLabel->setStyleSheet("QLabel {font-weight: 400; font-size: 16px}");
    uavNodeNumInput = new QLineEdit;
    uavNodeConfirm = new QPushButton("confirm");
    uavNodeConfirm->setEnabled(false);
    auto hLayout1 = new QHBoxLayout;
    hLayout1->addWidget(uavNodeNumInput);
    hLayout1->addWidget(uavNodeConfirm);
    formLayout->addRow(uavNodeLabel);
    formLayout->addRow("node num:", hLayout1);
    uavConfigList = new QListWidget;
    formLayout->addRow(uavConfigList);

    auto usvNodeLabel = new QLabel("2. USV");
    usvNodeLabel->setStyleSheet("QLabel {font-weight: 400; font-size: 16px}");
    usvNodeNumInput = new QLineEdit;
    usvNodeConfirm = new QPushButton("confirm");
    usvNodeConfirm->setEnabled(false);
    auto hLayout2 = new QHBoxLayout;
    hLayout2->addWidget(usvNodeNumInput);
    hLayout2->addWidget(usvNodeConfirm);
    formLayout->addRow(usvNodeLabel);
    formLayout->addRow("node num:", hLayout2);
    usvConfigList = new QListWidget;
    formLayout->addRow(usvConfigList);

    auto auvNodeLabel = new QLabel("3. AUV");
    auvNodeLabel->setStyleSheet("QLabel {font-weight: 400; font-size: 16px}");
    auvNodeNumInput = new QLineEdit;
    auvNodeConfirm = new QPushButton("confirm");
    auvNodeConfirm->setEnabled(false);
    auto hLayout3 = new QHBoxLayout;
    hLayout3->addWidget(auvNodeNumInput);
    hLayout3->addWidget(auvNodeConfirm);
    formLayout->addRow(auvNodeLabel);
    formLayout->addRow("node num:", hLayout3);
    auvConfigList = new QListWidget;
    formLayout->addRow(auvConfigList);

    // channel model
    auto channelModelTitle = new QLabel("Channel Model");
    channelModelTitle->setStyleSheet("QLabel {font-weight: bold; font-size: 20px}");
    formLayout->addRow(channelModelTitle);
    auto channelModelBox = new QComboBox;
    QStringList modeOptions;
    modeOptions << "射频无线信道" << "水声通信信道" << "水下光通信信道" << "卫星微波信道";
    channelModelBox->addItems(modeOptions);
    formLayout->addRow("channel model:", channelModelBox);

    // tail buttons
    btnToSimulation = new QPushButton("start");
    btnGoBack = new QPushButton("back");
    auto buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    buttonLayout->addWidget(btnGoBack);
    buttonLayout->addWidget(btnToSimulation);
    formLayout->addRow(buttonLayout);

    setLayout(formLayout);
    setMinimumSize(QSize(800, 600));
    setWindowTitle("New Simulation Configs");
}

void Configuration::initSignalSlots() {
    connect(uavNodeNumInput, &QLineEdit::textChanged, this, [=](const QString& value) {
        QRegularExpression regex("^[0-9]+$");
        uavNodeConfirm->setEnabled(regex.match(value).hasMatch());
    });
    connect(usvNodeNumInput, &QLineEdit::textChanged, this, [=](const QString& value) {
        QRegularExpression regex("^[0-9]+$");
        usvNodeConfirm->setEnabled(regex.match(value).hasMatch());
    });
    connect(auvNodeNumInput, &QLineEdit::textChanged, this, [=](const QString& value) {
        QRegularExpression regex("^[0-9]+$");
        auvNodeConfirm->setEnabled(regex.match(value).hasMatch());
    });
    connect(uavNodeConfirm, &QPushButton::clicked, this, [=] {
        int nodeNum = uavNodeNumInput->text().toInt();
        if (uavConfigList->count() > 0) {
            uavConfigList->clear();
        }
        for (int i = 0; i < nodeNum; ++i) {
            auto item = new QListWidgetItem(uavConfigList);
            auto itemWidget = new NodeConfigItem(i+1);
            item->setSizeHint(itemWidget->sizeHint());
            uavConfigList->setItemWidget(item, itemWidget);
        }
    });
    connect(usvNodeConfirm, &QPushButton::clicked, this, [=] {
        int nodeNum = usvNodeNumInput->text().toInt();
        if (usvConfigList->count() > 0) {
            usvConfigList->clear();
        }
        for (int i = 0; i < nodeNum; ++i) {
            auto item = new QListWidgetItem(usvConfigList);
            auto itemWidget = new NodeConfigItem(i+1);
            item->setSizeHint(itemWidget->sizeHint());
            usvConfigList->setItemWidget(item, itemWidget);
        }
    });
    connect(auvNodeConfirm, &QPushButton::clicked, this, [=] {
        int nodeNum = auvNodeNumInput->text().toInt();
        if (auvConfigList->count() > 0) {
            auvConfigList->clear();
        }
        for (int i = 0; i < nodeNum; ++i) {
            auto item = new QListWidgetItem(auvConfigList);
            auto itemWidget = new NodeConfigItem(i+1);
            item->setSizeHint(itemWidget->sizeHint());
            auvConfigList->setItemWidget(item, itemWidget);
        }
    });
    connect(btnGoBack, &QPushButton::clicked, this, [=] {
        auto *next = new Welcome;
        close();
        next->show();
    });
    connect(btnToSimulation, &QPushButton::clicked, this, [=] {
        auto *next = new FourGrid;
        close();
        next->show();
    });
}
