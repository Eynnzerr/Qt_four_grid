//
// Created by eynnzerr on 23-10-30.
//
#include "welcome.h"

Welcome::Welcome(QWidget *parent) {
    setupUI();
    initSignalSlots();
}

void Welcome::setupUI() {
    title = new QLabel("Welcome to Net Simulator");
    title->setStyleSheet("QLabel {font-weight: bold; font-size: 27px}");

    auto subtitleContainer = new QHBoxLayout;
    subtitle = new QLabel("Create configuration for new simulation\nOr open existing configuration from disk.");
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
    auto newText = new QLabel("New Simulation");
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
    auto historyText = new QLabel("Open From Disk");
    auto historyButtonContainer = new QVBoxLayout;
    historyButtonContainer->setAlignment(Qt::AlignCenter);
    historyButtonContainer->setSpacing(8);
    historyButtonContainer->addLayout(buttonCenterLayout2);
    historyButtonContainer->addWidget(historyText);

    auto *buttonLayout = new QHBoxLayout;
    buttonLayout->setSpacing(8);
    buttonLayout->addLayout(newButtonContainer);
    buttonLayout->addLayout(historyButtonContainer);

    auto *vLayout = new QVBoxLayout;
    vLayout->setSpacing(20);
    vLayout->setAlignment(Qt::AlignCenter);
    vLayout->addWidget(title);
    vLayout->addLayout(subtitleContainer);
    vLayout->addLayout(buttonLayout);

    setLayout(vLayout);
    setMinimumSize(640, 480);

    QFile qss(":/res/welcome.qss");
    if (qss.open(QFile::ReadOnly)) {

        QString stylesheet = QLatin1String(qss.readAll());
        setStyleSheet(stylesheet);
        qss.close();
    }
}

void Welcome::initSignalSlots() {
    connect(btnNew, &QPushButton::clicked, [=] {
        auto *next = new FourGrid;
        close();
        next->show();
    });
    connect(btnHistory, &QPushButton::clicked, [=] {
        QString dlgTitle="choose existing config file"; //对话框标题
        QString filter="(*.json)"; //文件过滤器
        QString fileName = QFileDialog::getOpenFileName(this,dlgTitle,"/",filter);
        if (fileName != nullptr) {
            auto *next = new FourGrid;
            close();
            next->show();
        }
    });
}

Welcome::~Welcome() = default;
