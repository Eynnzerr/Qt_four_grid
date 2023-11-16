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
    // auto formLayout = new QFormLayout;
    // formLayout->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);

    auto verticalLayout = new QVBoxLayout;

    // simulationName = new QLineEdit;
    // simulationName->setPlaceholderText("默认时间戳命名");
    // formLayout->addRow("仿真名称:", simulationName);

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
    verticalLayout->addWidget(divider);
    verticalLayout->addWidget(filePreview);
    // formLayout->addRow(headLayout);
    // formLayout->addRow("节点规模:", boxNodeNum);
    // formLayout->addRow(divider);
    // formLayout->addRow(filePreview);

    //setLayout(formLayout);
    setLayout(verticalLayout);
    setMinimumSize(QSize(800, 600));
    setWindowTitle("网络场景设置");
}

void Configuration::setupUI() {

    auto formLayout = new QFormLayout;
    formLayout->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);

    simulationName = new QLineEdit;
    formLayout->addRow("仿真名称:", simulationName);

    // node default_configs
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

    // channel default_configs
    auto channelConfigTitle = new QLabel("链路参数");
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
            currentConfigFile = new QFile("../default_configs/nodes/pos_configure_dense_test.json");
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
        // QString fileName = simulationName->text().isEmpty() ? QString::fromStdString(oss.str()) : simulationName->text();
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
        // TODO: 调用脚本生成拓扑图
        filePreview->setPlainText("正在生成拓扑图……");
        filePreview->setReadOnly(true);
        repaint();
        auto command = "./ns3.38-a-finish_get_topo-debug --nodeConfigPath=" + configPath;
        system(command.toLatin1().data());
        auto *next = new TopoDisplay(&fileName, "./A-FINISH-2d-plot.json"); // TODO 拓扑绘图文件名约定死
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
