//
// Created by eynnzerr on 23-11-10.
//

#ifndef FOUR_GRID_COMPLETEPAGE_H
#define FOUR_GRID_COMPLETEPAGE_H

#include <QWidget>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPlainTextEdit>
#include <unistd.h>
#include <sys/wait.h>
#include "fourgrid.h"
#include "RealSimPage.h"
#include <iostream>
#include <sys/stat.h>
#include <fcntl.h>
#include <thread>

class CompletePage: public QWidget {
    Q_OBJECT
private:
    bool isRealSim;
    char *tracePath;
    QString nodeConfigPath;
    QString streamConfigPath;

    QLabel *title;
    QProgressBar *progressBar;
    QPushButton *btnFinish;
    QPushButton *btnStart;
    QPlainTextEdit *scriptOutput;

    void setupUI();
    void initSignalSlots();
    void setupPipe();

public:
    explicit CompletePage(QString *simulationName, bool isRealSim = false, QWidget *parent = nullptr);
};


#endif //FOUR_GRID_COMPLETEPAGE_H
