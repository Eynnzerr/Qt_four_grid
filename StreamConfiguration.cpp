//
// Created by eynnzerr on 23-11-10.
//

#include "StreamConfiguration.h"

StreamConfiguration::StreamConfiguration(QWidget *parent) {
    setupUI();
    initSignalSlots();
}

StreamConfiguration::StreamConfiguration(QString *simulationName, QWidget *parent) {
    this->simulationName = simulationName;
    setupUI();
    initSignalSlots();
}

void StreamConfiguration::setupUI() {
    auto verticalLayout = new QVBoxLayout;

    auto label = new QLabel("流配置:");
    boxStreamConfig = new QComboBox;
    QStringList numOptions;
    numOptions << "预设配置 1" << "预设配置 2" << "预设配置 3";
    boxStreamConfig->addItems(numOptions);
    boxStreamConfig->setCurrentIndex(-1);
    boxStreamConfig->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    btnGoBack = new QPushButton("上一步");
    btnToSimulation = new QPushButton("开始仿真");
    btnToSimulation->setStyleSheet("QPushButton {background-color: deepskyblue; color: white;}");
    btnToSimulation->setEnabled(false); // disabled by default unless configuration is selected.

    auto headLayout = new QHBoxLayout;
    headLayout->addWidget(label);
    headLayout->addWidget(boxStreamConfig);
    headLayout->addStretch();
    // headLayout->addWidget(btnGoBack);
    headLayout->addWidget(btnToSimulation);

    auto divider = new QFrame(this);
    divider->setFrameShape(QFrame::HLine);
    divider->setStyleSheet("QFrame { min-height:3px }");

    filePreview = new QPlainTextEdit;

    verticalLayout->addLayout(headLayout);
    verticalLayout->addWidget(divider);
    verticalLayout->addWidget(filePreview);

//    boxRouterOption = new QComboBox;
//    QStringList routerOptions;
//    routerOptions << "智能路由" << "传统路由";
//    boxRouterOption->addItems(routerOptions);
//    boxRouterOption->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    // auto formLayout = new QFormLayout;
    // formLayout->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);
    // formLayout->addRow("流配置:", boxStreamConfig);
    // formLayout->addRow("路由选择:", boxRouterOption);
    // formLayout->addRow(filePreview);
    // formLayout->addRow(buttonLayout);
    // setLayout(formLayout);

    setLayout(verticalLayout);
    setMinimumSize(QSize(800, 600));
    setWindowTitle("业务流量配置");
}

void StreamConfiguration::initSignalSlots() {
    connect(boxStreamConfig, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [=](int index) {
        if (index >= 0) {
            btnToSimulation->setEnabled(true);
        }
        // TODO 按照用户设置规模从对应预设配置文件中随机选择
        if (currentConfigFile != nullptr) {
            filePreview->clear();
            currentConfigFile = nullptr;
        }
        if (index == 0) {
            currentConfigFile = new QFile("../default_configs/stream/stream_flow_sparse.json");
            if (currentConfigFile->open(QIODevice::ReadOnly)) {
                QTextStream in(currentConfigFile);
                filePreview->setPlainText(in.readAll());
                currentConfigFile->close();
            }
        } if (index == 1) {
            currentConfigFile = new QFile("../default_configs/stream/stream_flow_common.json");
            if (currentConfigFile->open(QIODevice::ReadOnly)) {
                QTextStream in(currentConfigFile);
                filePreview->setPlainText(in.readAll());
                currentConfigFile->close();
            }
        } else if (index == 2) {
            currentConfigFile = new QFile("../default_configs/stream/stream_flow_dense.json");
            if (currentConfigFile->open(QIODevice::ReadOnly)) {
                QTextStream in(currentConfigFile);
                filePreview->setPlainText(in.readAll());
                currentConfigFile->close();
            }
        }
    });

    connect(btnToSimulation, &QPushButton::clicked, this, [=] {
        // 保存当前所选配置
        if (boxStreamConfig->currentIndex() >= 0) {
            auto configFile = new QFile(QString("../history_configs/stream/") + simulationName + ".json");
            if (configFile->open(QIODevice::WriteOnly)) {
                QTextStream out(configFile);
                out << filePreview->toPlainText();
                configFile->close();
            }
        }

        auto *next = new CompletePage(simulationName);
        close();
        next->show();
    });

    connect(btnGoBack, &QPushButton::clicked, this, [=] {
        auto *next = new TopoDisplay;
        close();
        next->show();
    });
}
