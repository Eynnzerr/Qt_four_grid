//
// Created by eynnzerr on 23-11-10.
//

#include "TopoDisplay.h"

TopoDisplay::TopoDisplay(QWidget *parent) {
    timer = new QTimer(this);
    setupUI();

    initSignalSlots();
}

TopoDisplay::TopoDisplay(QString *simulationName, char *topoFilePath, bool isRealSim, QWidget *parent) {
    this->simulationName = simulationName;
    this->topoFilePath = topoFilePath;
    this->isRealSim = isRealSim;
    timer = new QTimer(this);

    setupUI();
    initSignalSlots();
}

void TopoDisplay::setupUI() {
    btnStart = new QPushButton("开始");
    btnStop = new QPushButton("暂停");
    btnGoBack = new QPushButton("上一步");
    btnGoNext = new QPushButton("下一步");
    btnGoNext->setStyleSheet("QPushButton {background-color: deepskyblue; color: white;}");

    auto divider = new QFrame(this);
    divider->setFrameShape(QFrame::HLine);
    divider->setStyleSheet("QFrame { min-height:3px }");

    topoWidget = new TopoWidget(topoFilePath);

    currentTime = 0;
    endTime = topoWidget->getTotalTime() * 100;
    progressBar = new QProgressBar;
    progressBar->setRange(0, endTime);

    auto buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(btnStart);
    buttonLayout->addWidget(btnStop);
    // buttonLayout->addStretch();
    buttonLayout->addWidget(progressBar);
    buttonLayout->addWidget(btnGoBack);
    buttonLayout->addWidget(btnGoNext);

    auto vLayout = new QVBoxLayout;
    vLayout->addLayout(buttonLayout);
    vLayout->addWidget(divider);
    vLayout->addWidget(topoWidget);

    setLayout(vLayout);
    setMinimumSize(800, 600);
    setWindowTitle("拓扑预览");
}

void TopoDisplay::initSignalSlots() {
    connect(timer, &QTimer::timeout, this, [=] {
        currentTime ++;
        if (currentTime >= endTime) {
            // cancel connection with timer and stop timer.
            timer->stop();
            btnStart->setEnabled(false);
            btnStop->setEnabled(false);
        } else {
            topoWidget->updatePosition();
        }
        progressBar->setValue(currentTime);
    });
    connect(btnStart, &QPushButton::clicked, [=] {
        timer->start(10);
    });
    connect(btnStop, &QPushButton::clicked, [=] {
        timer->stop();
    });
    connect(btnGoBack, &QPushButton::clicked, [=] {
        auto *next = new Configuration;
        close();
        next->show();
    });
    connect(btnGoNext, &QPushButton::clicked, [=] {
        auto *next = new StreamConfiguration(simulationName, isRealSim);
        close();
        next->show();
    });
}

void TopoDisplay::resizeEvent(QResizeEvent *event) {
    topoWidget->setCoefficient(event->size().width(), event->size().height());
}
