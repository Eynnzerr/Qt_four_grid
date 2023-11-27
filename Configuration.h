//
// Created by eynnzerr on 23-10-30.
//

#ifndef FOUR_GRID_CONFIGURATION_H
#define FOUR_GRID_CONFIGURATION_H

#include <chrono>
#include <sstream>
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
#include <QPlainTextEdit>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include "TopoDisplay.h"
#include "welcome.h"

class Configuration: public QWidget {
    Q_OBJECT
private:
    bool isRealSim = false;
    QFile *currentConfigFile = nullptr;
    QString fileName;

    QComboBox *boxNodeNum;
    QPlainTextEdit *filePreview;
    QPushButton *btnToSimulation;

    QLineEdit *fromNodeId;
    QLineEdit *toNodeId;

    void setupNewUI();
    void initSignalSlots();
    void closeEvent(QCloseEvent *event) override;
    void showEvent(QShowEvent *event) override;
public:
    explicit Configuration(bool isRealSim = false, QWidget *parent = nullptr);
    ~Configuration() override;
};


#endif //FOUR_GRID_CONFIGURATION_H
