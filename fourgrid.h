#ifndef FOURGRID_H
#define FOURGRID_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QProgressBar>
#include <QGridLayout>
#include <QFormLayout>
#include <QTimer>
#include <QPlainTextEdit>
#include "widget.h"
#include "chartWidget.h"


class FourGrid : public QWidget
{
    Q_OBJECT

private:
    char *tracePath;
    QFrame *frames[2][2]; // TODO DELETE
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

    ChartWidget *chart;
    Widget *flowGraph;

    // void setupUI();
    void setupNewUI();
    void initSignalSlots();
    static void addContentToFrame(QFrame *frame, QWidget *widget);
    void addContentToFrame(QWidget *widget, int row, int col);


    void resizeEvent(QResizeEvent *event) override;

public:
    explicit FourGrid(QWidget *parent = nullptr);
    explicit FourGrid(char *tracePath, QWidget *parent = nullptr);
    ~FourGrid() override;

public slots:
    void onTimeChanged();
};
#endif // FOURGRID_H
