//
// Created by eynnzerr on 23-11-10.
//

#ifndef FOUR_GRID_STREAMCONFIGURATION_H
#define FOUR_GRID_STREAMCONFIGURATION_H

#include <QWidget>
#include <QFile>
#include <QLineEdit>
#include <QComboBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QFormLayout>
#include <QTextStream>
#include "TopoDisplay.h"
#include "CompletePage.h"

class StreamConfiguration: public QWidget {
    Q_OBJECT
private:
    QString *simulationName = nullptr;
    QFile *currentConfigFile = nullptr;
    bool isRealSim = false;

    QComboBox *boxStreamConfig;
    QPlainTextEdit *filePreview;
    QComboBox *boxRouterOption;

    QPushButton *btnToSimulation;
    QPushButton *btnGoBack;

    void setupUI();
    void initSignalSlots();

public:
    explicit StreamConfiguration(QWidget *parent = nullptr);
    explicit StreamConfiguration(QString *simulationName, bool isRealSim = false, QWidget *parent = nullptr);
};


#endif //FOUR_GRID_STREAMCONFIGURATION_H
