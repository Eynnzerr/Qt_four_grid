//
// Created by eynnzerr on 23-11-21.
//

#ifndef FOUR_GRID_REALSIMPAGE_H
#define FOUR_GRID_REALSIMPAGE_H

#include <QWidget>
#include <QPushButton>
#include <QProgressBar>
#include <QPlainTextEdit>
#include <QGridLayout>
#include "widget.h"
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>
#include <functional>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

typedef websocketpp::client<websocketpp::config::asio_client> client;
typedef client::message_ptr message_ptr;

class RealSimPage: public QWidget {
    Q_OBJECT
private:
    client websocketClient;
    client::connection_ptr connection;

    char *tracePath;
    QTimer *timer;
    int currentTime;
    int totalTime;

    QPushButton *btnStart;
    QPushButton *btnStop;
    QProgressBar *progressBar;

    QFrame *leftFrame;
    QFrame *rightTopFrame;
    QFrame *rightBottomFrame;

    QPlainTextEdit *logOutput;

    Widget *flowGraph;

    void setupNewUI();
    void initSignalSlots();
    void setupWebsocketClient();
    static void addContentToFrame(QFrame *frame, QWidget *widget);
    void resizeEvent(QResizeEvent *event) override;

public:
    explicit RealSimPage(char *tracePath, QWidget *parent = nullptr);
    ~RealSimPage() override;

    // websocket callbacks
    void on_message(client* c, websocketpp::connection_hdl hdl, message_ptr msg);
    void on_open(client* c, websocketpp::connection_hdl hdl);
    void on_close(client* c, websocketpp::connection_hdl hdl);
};


#endif //FOUR_GRID_REALSIMPAGE_H
