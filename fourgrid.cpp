#include "fourgrid.h"

FourGrid::FourGrid(QWidget *parent)
    : QWidget(parent)
{
    // set up timer
    // currentTime = 0;
    // totalTime = 10000;
    timer = new QTimer(this);
    setupNewUI();
    initSignalSlots();
    setWindowTitle("network simulation panel");
}

FourGrid::FourGrid(char *tracePath, QWidget *parent) {
    timer = new QTimer(this);
    this->tracePath = tracePath;

    setupNewUI();
    initSignalSlots();
    setWindowTitle("图形化结果展示");
}

FourGrid::~FourGrid()
= default;

void FourGrid::setupNewUI() {
    auto *fourGrids = new QGridLayout;

    // 左侧流量展示图
    leftFrame = new QFrame;
    leftFrame->setFrameStyle(QFrame::Box);
    leftFrame->setStyleSheet("background-color:white;");
    leftFrame->setFrameShape(QFrame::Panel);
    leftFrame->setLineWidth(2);
    fourGrids->addWidget(leftFrame, 0, 0, 2, 1);

    // 右上曲线图
    rightTopFrame = new QFrame;
    rightTopFrame->setFrameStyle(QFrame::Box);
    rightTopFrame->setStyleSheet("background-color:white;");
    rightTopFrame->setFrameShape(QFrame::Panel);
    rightTopFrame->setLineWidth(2);
    rightTopFrame->setFixedSize(400, 300);
    fourGrids->addWidget(rightTopFrame, 0, 1, 1, 1);

    // 右下日志
    rightBottomFrame = new QFrame;
    rightBottomFrame->setFrameStyle(QFrame::Box);
    rightBottomFrame->setStyleSheet("background-color:white;");
    rightBottomFrame->setFrameShape(QFrame::Panel);
    rightBottomFrame->setLineWidth(2);
    fourGrids->addWidget(rightBottomFrame, 1, 1, 1, 1);

    // 安装具体控件
    flowGraph = new Widget(1, 1, tracePath);
    addContentToFrame(leftFrame, flowGraph);

    chart = new ChartWidget(tracePath);
    addContentToFrame(rightTopFrame, chart);

    currentTime = 0;
    totalTime = flowGraph->getTotalTime() * 100;

    // TODO: 读取log_path，这里以后统一把json读取提到本控件中，给子控件按需传数据下去即可。
    QFile loadFile(tracePath);
    if (!loadFile.open(QIODevice::ReadOnly)) {
        qDebug() << "couldn't open trace file json.";
        return;
    }

    QByteArray allData = loadFile.readAll();
    loadFile.close();

    QJsonParseError jsonError;
    QJsonDocument jsonDoc(QJsonDocument::fromJson(allData, &jsonError));

    if (jsonError.error != QJsonParseError::NoError) {
        qDebug() << "json error!" << jsonError.errorString();
        return;
    }

    QJsonObject real_rootObj = jsonDoc.object();
    QJsonObject rootObj = real_rootObj["2d_message"].toObject();
    QString logFilePath = rootObj.value("log_path").toString();

    logOutput = new QPlainTextEdit;
    logOutput->setReadOnly(true);
    auto logFile = new QFile(logFilePath);
    if (logFile->open(QIODevice::ReadOnly)) {
        QTextStream in(logFile);
        logOutput->setPlainText(in.readAll());
        logFile->close();
    } else {
        logOutput->setPlainText("日志文件读取失败。");
    }
    rightBottomFrame->setFixedSize(400, 300);
    addContentToFrame(rightBottomFrame, logOutput);

    // set up header.
    auto header = new QHBoxLayout;
    header->setSpacing(4);
    btnStart = new QPushButton("开始");
    btnStop = new QPushButton("暂停");
    // progressBar = new QProgressBar;
    // progressBar->setRange(0, totalTime);
    header->addWidget(btnStart);
    header->addWidget(btnStop);
    header->addStretch();
    // header->addWidget(progressBar);

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

// must have been called after setupUI() is called.
void FourGrid::initSignalSlots()
{
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

void FourGrid::addContentToFrame(QFrame *frame, QWidget *widget)
{
    auto *layout = new QVBoxLayout(frame);
    layout->addWidget(widget);
    frame->setLayout(layout);
}

void FourGrid::addContentToFrame(QWidget *widget, int row, int col)
{
    QFrame *frame = frames[row][col];
    addContentToFrame(frame, widget);
}

void FourGrid::resizeEvent(QResizeEvent *event)
{
    qDebug() << "current height:" << event->size().height();
    flowGraph->setCoefficient(leftFrame->size().width(), leftFrame->size().height());
//    QFrame *frame = frames[0][0];
//    qDebug() << "widowSize changed! width = "
//                << frame->size().width()
//                << ", heigth= " << frame->size().height();
//    w1->setCoefficient(frame->size().width(), frame->size().height());
//    w2->setCoefficient(frame->size().width(), frame->size().height());
}

void FourGrid::onTimeChanged()
{
    currentTime += 1000;
    if (currentTime >= totalTime) {
        currentTime = totalTime;
        // cancel connection with timer and stop timer.
        timer->stop();
        disconnect(timer, SIGNAL(timeout()), this, SLOT(onTimeChanged()));
    }
    progressBar->setValue(currentTime);
}
