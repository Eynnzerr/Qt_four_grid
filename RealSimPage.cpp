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

    // 此时文件解析已就绪，可以开始建立连接。
    std::thread websocketThread([=](){
        setupWebsocketClient();
    });
    websocketThread.detach();
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
            btnStart->setEnabled(false);

            // CP: 仿真结束，向半实物进程发送停止消息
            QJsonObject stopMsg;
            QJsonObject data;
            data["msg"] = "simulation stopped.";
            stopMsg["code"] = 3;
            stopMsg["data"] = data;
            QJsonDocument doc(stopMsg);
            websocketClient.send(connection->get_handle(), doc.toJson().toStdString(), websocketpp::frame::opcode::text);
        } else {
            flowGraph->updatePosition();
        }
        progressBar->setValue(currentTime);
    });
    connect(btnStart, &QPushButton::clicked, this, [=](){
        if (flowGraph->getCurrentTime() == 0) {
            btnStart->setText("开始");
            // 发送正式开始消息
            QJsonObject startMsg;
            QJsonObject data;
            data["msg"] = "simulation started.";
            startMsg["code"] = 4;
            startMsg["data"] = data;
            QJsonDocument doc(startMsg);
            websocketClient.send(connection->get_handle(), doc.toJson().toStdString(), websocketpp::frame::opcode::text);

//            if (websocketClient.stopped()) {
//                // 重新发起连接
//                std::thread websocketThread([=](){
//                    setupWebsocketClient();
//                });
//                websocketThread.detach();
//            }
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

void RealSimPage::setupWebsocketClient() {
    std::string uri = "ws://192.168.120.235:9002";

    try {
        // 设置日志输出级别
        websocketClient.set_access_channels(websocketpp::log::alevel::all);
        websocketClient.clear_access_channels(websocketpp::log::alevel::frame_payload);

        // 初始化Asio
        websocketClient.init_asio();

        websocketClient.set_open_handler(bind(&RealSimPage::on_open, this, &websocketClient, std::placeholders::_1));
        websocketClient.set_close_handler(bind(&RealSimPage::on_close, this, &websocketClient, std::placeholders::_1));
        websocketClient.set_message_handler(bind(&RealSimPage::on_message, this, &websocketClient, std::placeholders::_1, std::placeholders::_2));

        // 创建一个连接
        websocketpp::lib::error_code ec;
        connection = websocketClient.get_connection(uri, ec);
        if (ec) {
            std::cout << "无法创建连接，原因: " << ec.message() << std::endl;
            QTimer::singleShot(0, this, [=]() {
                logOutput->appendPlainText(QString::fromStdString("错误：与半实物进程连接建立失败。原因：" + ec.message()));
            });
            return;
        }

        // 连接服务器
        websocketClient.connect(connection);

        // 开始Asio的IO循环
        websocketClient.run();
    } catch (websocketpp::exception const & e) {
        std::cout << e.what() << std::endl;
    } catch (...) {
        std::cout << "发生未知错误" << std::endl;
    }
}

void RealSimPage::on_message(client *c, websocketpp::connection_hdl hdl, message_ptr msg) {
    std::cout << "收到来自半实物进程消息: " << msg->get_payload() << std::endl;
    // 解析Json，读取数据
    QJsonParseError jsonError{};
    QJsonDocument jsonDoc(QJsonDocument::fromJson(QByteArray::fromStdString(msg->get_payload()), &jsonError));
    if (jsonError.error != QJsonParseError::NoError) {
        qDebug() << "json error!" << jsonError.errorString();
        return;
    }
    QJsonObject rootObj = jsonDoc.object();
    int code = rootObj["code"].toInt();
    QJsonObject data = rootObj["data"].toObject();
    if (code == 1) {
        // 收到转发流消息，显示在图上
        int stream_type = data["type"].toInt();
        flowGraph->addStreamToDisplay(stream_type);

        QTimer::singleShot(0, this, [=]() {
            std::ostringstream oss;
            oss << "第" << flowGraph->getCurrentTimeBySec() << "秒: 转发" << "流类型:" << stream_type;
            logOutput->appendPlainText(QString::fromStdString(oss.str()));
        });
    } else {

    }
}

void RealSimPage::on_open(client *c, websocketpp::connection_hdl hdl) {
    // 将各流延时序列序列化后发送给server
    std::cout << "client 连接建立！" << std::endl;
    std::string message = flowGraph->getSerializedStreamTimeTraces();
    c->send(hdl, message, websocketpp::frame::opcode::text);
}

void RealSimPage::on_close(client *c, websocketpp::connection_hdl hdl) {
    std::cout << "client 连接关闭！" << std::endl;
}