//
// Created by eynnzerr on 23-10-30.
//
#include "welcome.h"

Welcome::Welcome(QWidget *parent) {
    setupDialog();
    setupUI();
    initSignalSlots();
}

void Welcome::setupUI() {
    auto titleContainer = new QHBoxLayout;
    title = new QLabel("网络仿真器");
    title->setStyleSheet("QLabel {font-weight: bold; font-size: 27px}");
    titleContainer->addWidget(title);
    titleContainer->setAlignment(Qt::AlignCenter);

    auto subtitleContainer = new QHBoxLayout;
    subtitle = new QLabel("创建新仿真配置或打开已有配置文件。");
    subtitleContainer->addWidget(subtitle);
    subtitleContainer->setAlignment(Qt::AlignCenter);

    // TODO
    btnNew = new QPushButton;
    QIcon iconNew(":/res/new.png");
    btnNew->setIcon(iconNew);
    btnNew->setIconSize(QSize(32, 32));
    btnNew->setFixedSize(QSize(48, 48));
    auto buttonCenterLayout1 = new QHBoxLayout;
    buttonCenterLayout1->setAlignment(Qt::AlignCenter);
    buttonCenterLayout1->addWidget(btnNew);
    auto newText = new QLabel("新建配置");
    auto newButtonContainer = new QVBoxLayout;
    newButtonContainer->setAlignment(Qt::AlignCenter);
    newButtonContainer->setSpacing(8);
    newButtonContainer->addLayout(buttonCenterLayout1);
    newButtonContainer->addWidget(newText);

    btnHistory = new QPushButton;
    QIcon iconOpenFile(":/res/file_open.png");
    btnHistory->setIcon(iconOpenFile);
    btnHistory->setIconSize(QSize(32, 32));
    btnHistory->setFixedSize(QSize(48, 48));
    auto buttonCenterLayout2 = new QHBoxLayout;
    buttonCenterLayout2->setAlignment(Qt::AlignCenter);
    buttonCenterLayout2->addWidget(btnHistory);
    auto historyText = new QLabel("从磁盘打开");
    auto historyButtonContainer = new QVBoxLayout;
    historyButtonContainer->setAlignment(Qt::AlignCenter);
    historyButtonContainer->setSpacing(8);
    historyButtonContainer->addLayout(buttonCenterLayout2);
    historyButtonContainer->addWidget(historyText);

    btnRealSim = new QPushButton;
    QIcon iconRealSim(":/res/computer.png");
    btnRealSim->setIcon(iconRealSim);
    btnRealSim->setIconSize(QSize(32, 32));
    btnRealSim->setFixedSize(QSize(48, 48));
    auto buttonCenterLayout3 = new QHBoxLayout;
    buttonCenterLayout3->setAlignment(Qt::AlignCenter);
    buttonCenterLayout3->addWidget(btnRealSim);
    auto realSimText = new QLabel("半实物仿真");
    auto realSimButtonContainer = new QVBoxLayout;
    realSimButtonContainer->setAlignment(Qt::AlignCenter);
    realSimButtonContainer->setSpacing(8);
    realSimButtonContainer->addLayout(buttonCenterLayout3);
    realSimButtonContainer->addWidget(realSimText);

    auto *buttonLayout = new QHBoxLayout;
    buttonLayout->setSpacing(16);
    buttonLayout->addLayout(newButtonContainer);
    buttonLayout->addLayout(historyButtonContainer);
    buttonLayout->addLayout(realSimButtonContainer);

    auto *vLayout = new QVBoxLayout;
    vLayout->setSpacing(20);
    vLayout->setAlignment(Qt::AlignCenter);
    vLayout->addLayout(titleContainer);
    vLayout->addLayout(subtitleContainer);
    vLayout->addLayout(buttonLayout);

    setLayout(vLayout);
    setMinimumSize(640, 480);
    setWindowTitle("欢迎使用网络仿真器");

    QFile qss(":/res/welcome.qss");
    if (qss.open(QFile::ReadOnly)) {
        QString stylesheet = QLatin1String(qss.readAll());
        setStyleSheet(stylesheet);
        qss.close();
    }
}

void Welcome::initSignalSlots() {
    connect(btnNew, &QPushButton::clicked, [=] {
        QSettings settings("HUST", "FourGrid");
        settings.setValue("is_real_sim", false);

        auto *next = new Configuration;
        close();
        next->show();
    });
    connect(btnHistory, &QPushButton::clicked, [=] {
        dialog->exec();
    });
    connect(btnRealSim, &QPushButton::clicked, [=] {
//        QString dlgTitle="选择已有配置文件"; //对话框标题
//        QString filter="(*.json)"; //文件过滤器
//        QString fileName = QFileDialog::getOpenFileName(this,dlgTitle,"/",filter);
//        if (fileName != nullptr) {
//            auto *next = new RealSimPage(fileName.toLatin1().data());
//            close();
//            next->show();
//        }
        QSettings settings("HUST", "FourGrid");
        settings.setValue("is_real_sim", true);

        auto *next = new Configuration(true);
        close();
        next->show();
    });
}

void Welcome::setupDialog() {
    dialog = new QDialog;
    buttonGroup = new QButtonGroup;
    auto toSoftwareSim = new QRadioButton("全软件仿真");
    auto toHalfRealSim = new QRadioButton("半实物仿真");
    buttonGroup->addButton(toSoftwareSim, 0);
    buttonGroup->addButton(toHalfRealSim, 1);
    toSoftwareSim->setChecked(true);
    auto dialogLayout = new QVBoxLayout;
    dialogLayout->addWidget(toSoftwareSim);
    dialogLayout->addWidget(toHalfRealSim);
    auto dialogButtons = new QHBoxLayout;
    auto btnConfirm = new QPushButton("确定");
    dialogButtons->addStretch();
    dialogButtons->addWidget(btnConfirm);
    dialogLayout->addLayout(dialogButtons);
    dialog->setLayout(dialogLayout);
    dialog->setWindowTitle("选择历史仿真类型");

    connect(btnConfirm, &QPushButton::clicked, this, [=] {
        dialog->close();

        QString dlgTitle="选择已有配置文件";
        QString filter="(*.json)";
        QString fileName = QFileDialog::getOpenFileName(this,dlgTitle,"/",filter);
        if (fileName != nullptr) {
            QWidget *next;
            if (buttonGroup->checkedId() == 0) {
                QSettings settings("HUST", "FourGrid");
                settings.setValue("is_real_sim", false);

                next = new FourGrid(fileName.toLatin1().data());
            } else {
                QSettings settings("HUST", "FourGrid");
                settings.setValue("is_real_sim", true);

                next = new RealSimPage(fileName.toLatin1().data());
            }
            close();
            next->show();
        }
    });
}

Welcome::~Welcome() = default;
