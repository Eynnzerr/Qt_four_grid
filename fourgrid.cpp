#include "fourgrid.h"

FourGrid::FourGrid(QWidget *parent)
    : QWidget(parent)
{
    // set up timer
    currentTime = 0;
    totalTime = 10000;
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimeChanged()));

    setupUI();
    initSignalSlots();
    setWindowTitle("network simulation panel");
}

FourGrid::~FourGrid()
= default;

void FourGrid::setupUI()
{
    // set up four grids and sub widgets
    auto *fourGrids = new QGridLayout;

    for (int row = 0; row < 2; ++row)
    {
        for (int col = 0; col < 2; ++col)
        {
            auto *frame = new QFrame;
            frame->setFrameStyle(QFrame::Box);
            frame->setStyleSheet("background-color:white;");
            frame->setFrameShape(QFrame::Panel);
            frame->setLineWidth(2);
            frames[row][col] = frame;
            fourGrids->addWidget(frame, row, col);
        }
    }

    // TODO: or setup child widget here.
    // label1 = new QLabel("test");
    // addContentToFrame(label1, 0, 0);
    w1 = new Widget(0,1);
    addContentToFrame(w1, 0, 0);

    w2 = new Widget(1,0);
    addContentToFrame(w2, 1, 0);



    label2 = new QLabel("test");
    addContentToFrame(label2, 0, 1);
    // label3 = new QLabel("test");
    // addContentToFrame(label3, 1, 0);
    label4 = new QLabel("test");
    addContentToFrame(label4, 1, 1);

    // set up header.
    auto *header = new QHBoxLayout;
    header->setSpacing(4);
    btnStart = new QPushButton("start");
    btnStop = new QPushButton("stop");
    progressBar = new QProgressBar;
    progressBar->setRange(0, totalTime);
    header->addWidget(btnStart);
    header->addWidget(btnStop);
    header->addWidget(progressBar);

    auto *vLayout = new QVBoxLayout;
    vLayout->setSpacing(8);
    vLayout->addLayout(header);
    vLayout->addLayout(fourGrids);

    setLayout(vLayout);
    setMinimumSize(800, 600);
}

// must have been called after setupUI() is called.
void FourGrid::initSignalSlots()
{
    // initialize diy signals and connect to slots.
    connect(btnStart, &QPushButton::clicked, this, [=](){
        timer->start(1000);
    });
    connect(btnStop, &QPushButton::clicked, this, [=](){
        timer->stop();
    });
}

void FourGrid::addContentToFrame(QFrame *frame, QWidget *widget)
{
    auto *layout = new QVBoxLayout(frame);
    // layout->setAlignment(Qt::AlignCenter);
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
    QFrame *frame = frames[0][0];
    qDebug() << "widowSize changed! width = "
                << frame->size().width()
                << ", heigth= " << frame->size().height();
    w1->setCoefficient(frame->size().width(), frame->size().height());
    w2->setCoefficient(frame->size().width(), frame->size().height());
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

    // TODO sub widgets should design their own slots and connect to timeout() of timer.
    // label1->setText(QString::number(currentTime / 1000));
    label2->setText(QString::number(currentTime / 1000));
    // label3->setText(QString::number(currentTime / 1000));
    label4->setText(QString::number(currentTime / 1000));
}
