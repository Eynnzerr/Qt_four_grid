//
// Created by eynnzerr on 23-11-1.
//

#ifndef FOUR_GRID_STREAMCONFIGITEM_H
#define FOUR_GRID_STREAMCONFIGITEM_H

#include <QWidget>
#include <QHBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QRegExp>
#include <QRegExpValidator>
#include <QJsonObject>

class StreamConfigItem: public QWidget {
    Q_OBJECT
private:    // 流种类，发送/接收节点序号，速度，起止时间
    int index;
    QComboBox *typeBox;
    QLineEdit *senderNode;
    QLineEdit *receiverNode;
    QLineEdit *streamRate;
    QLineEdit *startTime;
    QLineEdit *endTime;

    void setupUI();
    void initSignalSlots();

public:
    explicit StreamConfigItem(int index);
    int getType();
    int getSenderNode();
    int getReceiverNode();
    double getRate();
    int getStartTime();
    int getEndTime();
    QJsonObject toJsonObject();
};


#endif //FOUR_GRID_STREAMCONFIGITEM_H
