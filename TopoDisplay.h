//
// Created by eynnzerr on 23-11-10.
//

#ifndef FOUR_GRID_TOPODISPLAY_H
#define FOUR_GRID_TOPODISPLAY_H

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QTimer>
#include "Configuration.h"
#include "StreamConfiguration.h"
#include "TopoWidget.h"

class TopoDisplay: public QWidget {
    Q_OBJECT
private:
    int currentTime;
    int endTime;
    char *topoFilePath;
    QTimer *timer;
    QString *simulationName = nullptr;
    QPushButton *btnStart;
    QPushButton *btnStop;
    QPushButton *btnGoBack;
    QPushButton *btnGoNext;
    TopoWidget *topoWidget;
    QProgressBar *progressBar;

    void setupUI();
    void initSignalSlots();
    void resizeEvent(QResizeEvent *event) override;
public:
    explicit TopoDisplay(QWidget *parent = nullptr);
    explicit TopoDisplay(QString *simulationName, char *topoFilePath, QWidget *parent = nullptr);
};


#endif //FOUR_GRID_TOPODISPLAY_H
