//
// Created by eynnzerr on 23-10-30.
//

#include "Configuration.h"
#include <QChartView>

Configuration::Configuration(bool isRealSim, QWidget *parent): QWidget(parent)
{
    // setupUI();
    this->isRealSim = isRealSim;
    setupNewUI();
    initSignalSlots();
}

Configuration::~Configuration() = default;

void Configuration::setupNewUI() {
    auto verticalLayout = new QVBoxLayout;

    btnToSimulation = new QPushButton("下一步");
    btnToSimulation->setStyleSheet("QPushButton {background-color: deepskyblue; color: white;}");
    btnToSimulation->setEnabled(false); // disabled by default unless configuration is selected.

    auto label = new QLabel("节点规模:");
    boxNodeNum = new QComboBox;
    QStringList numOptions;
    numOptions << "稀疏规模(16个)" << "标准规模(32个)" << "密集规模(64个)";
    boxNodeNum->addItems(numOptions);
    boxNodeNum->setCurrentIndex(-1);
    boxNodeNum->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    auto headLayout = new QHBoxLayout;
    headLayout->addWidget(label);
    headLayout->addWidget(boxNodeNum);
    headLayout->addStretch();
    headLayout->addWidget(btnToSimulation);

    auto divider = new QFrame(this);
    divider->setFrameShape(QFrame::HLine);
    divider->setStyleSheet("QFrame { min-height:3px }");

    filePreview = new QPlainTextEdit;

    verticalLayout->addLayout(headLayout);
    if (isRealSim) {
        auto mapLayout = new QHBoxLayout;
        fromNodeId = new QLineEdit;
        fromNodeId->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        fromNodeId->setValidator(new QIntValidator);
        toNodeId = new QLineEdit;
        toNodeId->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        toNodeId->setValidator(new QIntValidator);
        mapLayout->addWidget(new QLabel("映射节点: 起始: "));
        mapLayout->addWidget(fromNodeId);
        mapLayout->addWidget(new QLabel("至: "));
        mapLayout->addWidget(toNodeId);
        mapLayout->addStretch();
        verticalLayout->addLayout(mapLayout);
    }
    verticalLayout->addWidget(divider);
    verticalLayout->addWidget(filePreview);

    setLayout(verticalLayout);
    setMinimumSize(QSize(800, 600));
    setWindowTitle("网络场景设置");
}

void Configuration::initSignalSlots() {
    connect(boxNodeNum, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [=](int index) {
        if (index >= 0) {
            btnToSimulation->setEnabled(true);
        }
        if (currentConfigFile != nullptr) {
            filePreview->clear();
            currentConfigFile = nullptr;
        }
        // TODO 按照用户设置规模从对应预设配置文件中随机选择
        if (index == 0) {
            currentConfigFile = new QFile("../default_configs/nodes/sparse/pos_configure_sparse1.json");
            if (currentConfigFile->open(QIODevice::ReadOnly)) {
                QTextStream in(currentConfigFile);
                filePreview->setPlainText(in.readAll());
                currentConfigFile->close();
            }
        } else if (index == 1) {
            currentConfigFile = new QFile("../default_configs/nodes/common/pos_configure_common1.json");
            if (currentConfigFile->open(QIODevice::ReadOnly)) {
                QTextStream in(currentConfigFile);
                filePreview->setPlainText(in.readAll());
                currentConfigFile->close();
            }
        } else if (index == 2) {
            currentConfigFile = new QFile("../default_configs/nodes/dense/pos_configure_dense2.json");
            if (currentConfigFile->open(QIODevice::ReadOnly)) {
                QTextStream in(currentConfigFile);
                filePreview->setPlainText(in.readAll());
                currentConfigFile->close();
            }
        }
    });

    connect(btnToSimulation, &QPushButton::clicked, this, [=] {
        // 保存当前所选配置
        // 自动化生成文件名
        auto now = std::chrono::system_clock::now();
        auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
        std::ostringstream oss;
        oss << timestamp << "_" << boxNodeNum->currentIndex();
        fileName = QString::fromStdString(oss.str());
        auto configPath = "../history_configs/nodes/" + fileName + ".json";

        if (boxNodeNum->currentIndex() >= 0) {
            auto configFile = new QFile(configPath);
            if (configFile->open(QIODevice::WriteOnly)) {
                QTextStream out(configFile);
                out << filePreview->toPlainText();
                configFile->close();
            }
        }

        if (isRealSim) {
            QSettings settings("HUST", "FourGrid");
            settings.setValue("from_node_id", fromNodeId->text().toInt());
            settings.setValue("to_node_id", toNodeId->text().toInt());
        }

        // 调用脚本生成拓扑图
        filePreview->setPlainText("正在生成拓扑图……");
        filePreview->setReadOnly(true);
        repaint();
        auto command = "./ns3.38-a-finish_get_topo-debug --nodeConfigPath=" + configPath;
        system(command.toLatin1().data());
        auto *next = new TopoDisplay(&fileName, "./A-FINISH-2d-plot.json", isRealSim); // 拓扑绘图文件名约定死
        close();
        next->show();
    });
}

void Configuration::closeEvent(QCloseEvent *event) {
//    QSettings settings("HUST", "FourGrid");
//    settings.setValue("simulation_name", simulationName->text());
//    settings.setValue("box_index", boxNodeNum->currentIndex());
//    settings.setValue("config_text", filePreview->toPlainText());
    QWidget::closeEvent(event);
}

void Configuration::showEvent(QShowEvent *event) {
//    QSettings settings("HUST", "FourGrid");
//    simulationName->setText(settings.value("simulation_name").toString());
//    boxNodeNum->setCurrentIndex(settings.value("box_index").toInt());
//    filePreview->setPlainText(settings.value("config_text").toString());
    QWidget::showEvent(event);
}
