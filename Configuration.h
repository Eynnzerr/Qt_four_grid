//
// Created by eynnzerr on 23-10-30.
//

#ifndef FOUR_GRID_CONFIGURATION_H
#define FOUR_GRID_CONFIGURATION_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QListWidget>
#include <QComboBox>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include "NodeConfigItem.h"
#include "StreamConfigItem.h"
#include "welcome.h"

class Configuration: public QWidget {
    Q_OBJECT
private:
    QPushButton *btnToSimulation;
    QPushButton *btnGoBack;

    QPushButton *btnAddNode;
    QPushButton *btnDeleteNode;
    QListWidget *nodeConfigList;

    QPushButton *btnAddStream;
    QPushButton *btnDeleteStream;
    QListWidget *streamConfigList;

    QLineEdit *underwaterComDistance;
    QLineEdit *wifiComDistance;
    QLineEdit *opticalComDistance;

    void setupUI();
    void initSignalSlots();
public:
    explicit Configuration(QWidget *parent = nullptr);
    ~Configuration() override;
};


#endif //FOUR_GRID_CONFIGURATION_H
