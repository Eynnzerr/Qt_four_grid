//
// Created by eynnzerr on 23-11-10.
//

#include "CompletePage.h"

CompletePage::CompletePage(QString *simulationName, bool isRealSim, QWidget *parent) {
    this->isRealSim = isRealSim;
    nodeConfigPath = QString("../history_configs/nodes/") + simulationName + ".json";
    streamConfigPath = QString("../history_configs/stream/") + simulationName + ".json";

    setupUI();
    initSignalSlots();
}

void CompletePage::setupUI() {

    title = new QLabel("配置已就绪，点击按钮开始仿真。");
    progressBar = new QProgressBar;
    progressBar->setRange(0, 100);
    btnStart = new QPushButton("开始仿真");
    btnFinish = new QPushButton("图形化展示");
    btnFinish->setEnabled(false);
    scriptOutput = new QPlainTextEdit;
    scriptOutput->setReadOnly(true);

    auto headLayout = new QHBoxLayout;
    headLayout->addWidget(title);
    headLayout->addStretch();
    headLayout->addWidget(btnStart);
    headLayout->addWidget(btnFinish);

    auto vLayout = new QVBoxLayout;
    vLayout->addLayout(headLayout);
    // vLayout->addWidget(progressBar);
    vLayout->addWidget(scriptOutput);

    setLayout(vLayout);
    setMinimumSize(800, 600);
    setWindowTitle("仿真日志结果");
}

void CompletePage::initSignalSlots() {
    connect(btnFinish, &QPushButton::clicked, this, [=] {
        QWidget *next;
        if (isRealSim) {
            next = new RealSimPage(tracePath);
        } else {
            next = new FourGrid(tracePath);
        }
        close();
        next->show();
    });
    connect(btnStart, &QPushButton::clicked, this, [=] {
        title->setText("仿真进行中……");
        btnStart->setEnabled(false);
        setupPipe();
    });
}

void CompletePage::setupPipe() {
    // 创建FIFO
    const char* fifoPath = "/tmp/myfifo";
    int ret = mkfifo(fifoPath, 0666);
    if (ret != 0) {
        perror("reader mkfifo failed");
    }

    // 调用ns3，启动仿真，输出文件路径写入管道
    std::thread cmdThread([=](){
        QString command = QString("./ns3_script --nodeConfigPath=") + nodeConfigPath + " --streamConfigPath=" + streamConfigPath;
        qDebug() << "node config path:" << nodeConfigPath;
        system(command.toLatin1().data());
    });
    cmdThread.detach();

    // 必须先开子线程让ns3打开管道一端，否则阻塞死锁
    int fifoFd = open(fifoPath, O_RDONLY);
    if (fifoFd < 0) {
        perror("reader can't open fifo");
    }

    // 从管道读取数据
    char buffer[1024];
    ssize_t bytesRead;

    while (true) {
        memset(buffer, 0, sizeof(buffer));
        bytesRead = read(fifoFd, buffer, sizeof(buffer) - 1); //阻塞等写入

        if (bytesRead > 0) {
            // 处理读取的数据
            std::string line;
            for (int i = 0; i < bytesRead; ++i) {
                if (buffer[i] == '\n') {
                    std::cout << "Received: " << line << std::endl;
                    scriptOutput->appendPlainText(QString::fromStdString(line));
                    repaint();
                    if (line.rfind("save to ", 0) == 0) {
                        std::string substring = line.substr(8);
                        const char *path = substring.c_str(); // temp object, must be copied.
                        qDebug() << "path:" << path << "   length:" << strlen(path);
                        tracePath = (char *) malloc(100);
                        strcpy(tracePath, path);
                        qDebug() << "tracePath is:" << tracePath;
                        // tracePath = "./A-FINISH-2d-plot.json";
                    }
                    line.clear();
                } else {
                    line += buffer[i];
                }
            }
        } else {
            // 没有数据可读，或者读取出错
            break;
        }
    }

    title->setText("仿真完成。");
    btnFinish->setEnabled(true);

    // 关闭文件描述符
    ::close(fifoFd);

    // 删除命名管道
    unlink(fifoPath);
}
