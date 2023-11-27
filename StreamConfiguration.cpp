//
// Created by eynnzerr on 23-11-10.
//

#include "StreamConfiguration.h"

StreamConfiguration::StreamConfiguration(QWidget *parent) {
    setupUI();
    initSignalSlots();
}

StreamConfiguration::StreamConfiguration(QString *simulationName, bool isRealSim, QWidget *parent) {
    this->simulationName = simulationName;
    this->isRealSim = isRealSim;
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

    // 是否为半实物仿真
    if (isRealSim) {
        QSettings settings("HUST", "FourGrid");
        boxStreamConfig->setCurrentIndex(0);
        btnToSimulation->setEnabled(true);
        QJsonObject rootObj;
        QJsonArray streams;
        QJsonObject oneStream;
        oneStream["stream_id"] = 1;
        oneStream["from_id"] = settings.value("from_node_id").toInt();
        oneStream["end_id"] = settings.value("to_node_id").toInt();
        oneStream["steam_type"] = 0;
        oneStream["begin_time"] = 4;
        oneStream["end_time"] = 30;
        streams.append(oneStream);
        rootObj["stream"] = streams;
        QJsonDocument doc(rootObj);
        filePreview->setPlainText(QString::fromStdString(doc.toJson().toStdString()));
    }

    verticalLayout->addLayout(headLayout);
    verticalLayout->addWidget(divider);
    verticalLayout->addWidget(filePreview);

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

        auto *next = new CompletePage(simulationName, isRealSim);
        close();
        next->show();
    });

    connect(btnGoBack, &QPushButton::clicked, this, [=] {
        auto *next = new TopoDisplay;
        close();
        next->show();
    });
}
