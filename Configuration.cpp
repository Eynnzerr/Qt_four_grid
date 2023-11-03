//
// Created by eynnzerr on 23-10-30.
//

#include "Configuration.h"

Configuration::Configuration(QWidget *parent): QWidget(parent)
{
    setupUI();
    initSignalSlots();
}

Configuration::~Configuration() = default;

void Configuration::setupUI() {
    auto formLayout = new QFormLayout;
    formLayout->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);

    // node configs
    auto nodeConfigTitle = new QLabel("节点参数");
    nodeConfigTitle->setStyleSheet("QLabel {font-weight: bold; font-size: 20px}");
    formLayout->addRow(nodeConfigTitle);

    btnAddNode = new QPushButton;
    btnAddNode->setIcon(QIcon(":/res/add.png"));
    btnAddNode->setStyleSheet("QPushButton {background: transparent; border: none} QPushButton:hover {background: grey; border: none}");
    btnDeleteNode = new QPushButton;
    btnDeleteNode->setIcon(QIcon(":/res/subtract.png"));
    btnDeleteNode->setStyleSheet("QPushButton {background: transparent; border: none} QPushButton:hover {background: grey; border: none}");
    auto hLayout1 = new QHBoxLayout;
    hLayout1->addWidget(btnAddNode);
    hLayout1->addWidget(btnDeleteNode);
    hLayout1->addStretch();
    formLayout->addRow(hLayout1);

    nodeConfigList = new QListWidget;
    nodeConfigList->setSelectionMode(QAbstractItemView::NoSelection);
    formLayout->addRow(nodeConfigList);
//    auto nodeConfigHeader = new QHBoxLayout;
//    auto nodeNumberHeader = new QLabel("序号");
//    auto nodeTypeHeader = new QLabel("节点种类");
//    auto movementHeader = new QLabel("运动方式");
//    auto positionHeader = new QLabel("初始位置");
//    nodeConfigHeader->addWidget(nodeNumberHeader);
//    nodeConfigHeader->addStretch();
//    nodeConfigHeader->addWidget(nodeTypeHeader);
//    nodeConfigHeader->addWidget(movementHeader);
//    nodeConfigHeader->addWidget(positionHeader);
//    auto itemHeader = new QListWidgetItem(nodeConfigList);
//    itemHeader->setSizeHint(nodeConfigHeader->sizeHint());

    // channel configs
    auto channelConfigTitle = new QLabel("信道参数");
    channelConfigTitle->setStyleSheet("QLabel {font-weight: bold; font-size: 20px}");
    formLayout->addRow(channelConfigTitle);

    btnAddStream = new QPushButton;
    btnAddStream->setIcon(QIcon(":/res/add.png"));
    btnAddStream->setStyleSheet("QPushButton {background: transparent; border: none} QPushButton:hover {background: grey; border: none}");
    btnDeleteStream = new QPushButton;
    btnDeleteStream->setIcon(QIcon(":/res/subtract.png"));
    btnDeleteStream->setStyleSheet("QPushButton {background: transparent; border: none} QPushButton:hover {background: grey; border: none}");
    auto hLayout2 = new QHBoxLayout;
    hLayout2->addWidget(btnAddStream);
    hLayout2->addWidget(btnDeleteStream);
    hLayout2->addStretch();
    formLayout->addRow(hLayout2);

    streamConfigList = new QListWidget;
    streamConfigList->setSelectionMode(QAbstractItemView::NoSelection);
    formLayout->addRow(streamConfigList);

    underwaterComDistance = new QLineEdit;
    wifiComDistance = new QLineEdit;
    opticalComDistance = new QLineEdit;
    formLayout->addRow("水声通信距离:", underwaterComDistance);
    formLayout->addRow("wifi通信距离:", wifiComDistance);
    formLayout->addRow("光通信距离:", opticalComDistance);

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
    connect(btnAddNode, &QPushButton::clicked, this, [=] {
        auto item = new QListWidgetItem(nodeConfigList);
        auto itemWidget = new NodeConfigItem(nodeConfigList->count());
        item->setSizeHint(itemWidget->sizeHint());
        nodeConfigList->setItemWidget(item, itemWidget);
    });
    connect(btnDeleteNode, &QPushButton::clicked, this, [=] {
        if (nodeConfigList->count() > 0) {
            QListWidgetItem *item = nodeConfigList->takeItem(nodeConfigList->count() - 1);
            delete item;
        }
    });

    connect(btnAddStream, &QPushButton::clicked, this, [=] {
        auto item = new QListWidgetItem(streamConfigList);
        auto itemWidget = new StreamConfigItem(streamConfigList->count());
        item->setSizeHint(itemWidget->sizeHint());
        streamConfigList->setItemWidget(item, itemWidget);
    });
    connect(btnDeleteStream, &QPushButton::clicked, this, [=] {
        if (streamConfigList->count() > 0) {
            QListWidgetItem *item = streamConfigList->takeItem(streamConfigList->count() - 1);
            delete item;
        }
    });

    connect(btnGoBack, &QPushButton::clicked, this, [=] {
        auto *next = new Welcome;
        close();
        next->show();
    });
    connect(btnToSimulation, &QPushButton::clicked, this, [=] {
        // 收集本页面全部写入信息，存入新建json文件中
        QJsonObject simulationConfig;
        QJsonArray nodeConfigs, streamConfigs;
        for (int i = 0; i < nodeConfigList->count(); ++i) {
            QListWidgetItem *listItem = nodeConfigList->item(i);
            if (listItem) {
                auto nodeConfigItem = (NodeConfigItem *) nodeConfigList->itemWidget(listItem);
                nodeConfigs.append(nodeConfigItem->toJsonObject());
            }
        }
        simulationConfig["node_config"] = nodeConfigs;
        for (int i = 0; i < streamConfigList->count(); ++i) {
            QListWidgetItem *listItem = streamConfigList->item(i);
            if (listItem) {
                auto streamConfigItem = (StreamConfigItem *) streamConfigList->itemWidget(listItem);
                streamConfigs.append(streamConfigItem->toJsonObject());
            }
        }
        simulationConfig["stream_config"] = streamConfigs;
        simulationConfig["submarine_distance"] = underwaterComDistance->text().toDouble();
        simulationConfig["wifi_distance"] = wifiComDistance->text().toDouble();
        simulationConfig["optic_distance"] = opticalComDistance->text().toDouble();

        QJsonDocument jsonDoc(simulationConfig);
        QFile file("data.json");
        if (file.open(QIODevice::WriteOnly)) {
            file.write(jsonDoc.toJson());
            file.close();
            qDebug() << "new config file saved successfully.";
        } else {
            qDebug() << "Failed to save new config file.";
        }

        auto *next = new FourGrid;
        close();
        next->show();
    });
}
