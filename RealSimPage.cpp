//
// Created by eynnzerr on 23-11-21.
//

#include "RealSimPage.h"

RealSimPage::RealSimPage(char *tracePath, QWidget *parent) {
    timer = new QTimer(this);
    this->tracePath = tracePath;

    setupNewUI();
    initSignalSlots();
    setWindowTitle("半实物仿真");
}

RealSimPage::~RealSimPage() = default;

void RealSimPage::setupNewUI() {
    auto *fourGrids = new QGridLayout;

    // 左侧流量展示图
    leftFrame = new QFrame;
    leftFrame->setFrameStyle(QFrame::Box);
    leftFrame->setStyleSheet("background-color:white;");
    leftFrame->setFrameShape(QFrame::Panel);
    leftFrame->setLineWidth(2);
    fourGrids->addWidget(leftFrame, 0, 0, 2, 1);

    // 右上
    rightTopFrame = new QFrame;
    rightTopFrame->setFrameStyle(QFrame::Box);
    rightTopFrame->setStyleSheet("background-color:white;");
    rightTopFrame->setFrameShape(QFrame::Panel);
    rightTopFrame->setLineWidth(2);
    rightTopFrame->setFixedSize(400, 300);
    fourGrids->addWidget(rightTopFrame, 0, 1, 1, 1);

    // 右下
    rightBottomFrame = new QFrame;
    rightBottomFrame->setFrameStyle(QFrame::Box);
    rightBottomFrame->setStyleSheet("background-color:white;");
    rightBottomFrame->setFrameShape(QFrame::Panel);
    rightBottomFrame->setLineWidth(2);
    fourGrids->addWidget(rightBottomFrame, 1, 1, 1, 1);

    // 安装具体控件
    flowGraph = new Widget(1, 0, tracePath);
    addContentToFrame(leftFrame, flowGraph);

    currentTime = 0;
    totalTime = flowGraph->getTotalTime() * 100;

    // 暂时性验证功能的组件
    auto testButton = new QPushButton("发送流");
    connect(testButton, &QPushButton::clicked, this, [=] {
        // TODO: 没想明白，为什么时间要由外界决定？应该由空间自身的currenttimestamp决定才合理！
       // flowGraph->drawLineForStreamAtTime(0, flowGraph->getCurrentTime());
       flowGraph->addStreamToDisplay(0);
    });
    addContentToFrame(rightTopFrame, testButton);

    logOutput = new QPlainTextEdit;
    logOutput->setReadOnly(true);
    rightBottomFrame->setFixedSize(400, 300);
    addContentToFrame(rightBottomFrame, logOutput);

    // set up header.
    auto header = new QHBoxLayout;
    header->setSpacing(4);
    btnStart = new QPushButton("开始");
    btnStop = new QPushButton("暂停");
    progressBar = new QProgressBar;
    progressBar->setRange(0, totalTime);
    header->addWidget(btnStart);
    header->addWidget(btnStop);
    // header->addStretch();
    header->addWidget(progressBar);

    // set up information.
    auto info = new QHBoxLayout;
    info->addWidget(new QLabel("trace文件路径:"));
    info->addWidget(new QLabel(tracePath));
    info->addStretch();

    auto *vLayout = new QVBoxLayout;
    vLayout->setSpacing(8);
    vLayout->addLayout(header);
    vLayout->addLayout(info);
    vLayout->addLayout(fourGrids);

    setLayout(vLayout);
    setFixedSize(1200, 700);
}

void RealSimPage::initSignalSlots() {
    connect(timer, &QTimer::timeout, this, [=] {
        currentTime ++;
        if (currentTime >= totalTime) {
            timer->stop();
            currentTime = 0;
            flowGraph->resetTimestamp();
            btnStart->setText("重启");
        } else {
            flowGraph->updatePosition();
        }
    });
    connect(btnStart, &QPushButton::clicked, this, [=](){
        if (flowGraph->getCurrentTime() == 0) {
            btnStart->setText("开始");
        }
        timer->start(10);
    });
    connect(btnStop, &QPushButton::clicked, this, [=](){
        timer->stop();
    });
}

void RealSimPage::addContentToFrame(QFrame *frame, QWidget *widget) {
    auto *layout = new QVBoxLayout(frame);
    layout->addWidget(widget);
    frame->setLayout(layout);
}

void RealSimPage::resizeEvent(QResizeEvent *event) {
    qDebug() << "current height:" << event->size().height();
    flowGraph->setCoefficient(leftFrame->size().width(), leftFrame->size().height());
}
