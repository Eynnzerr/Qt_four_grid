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

class RealSimPage: public QWidget {
    Q_OBJECT
private:
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
    static void addContentToFrame(QFrame *frame, QWidget *widget);
    void resizeEvent(QResizeEvent *event) override;

public:
    explicit RealSimPage(char *tracePath, QWidget *parent = nullptr);
    ~RealSimPage() override;
};


#endif //FOUR_GRID_REALSIMPAGE_H
