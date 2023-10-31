#ifndef FOURGRID_H
#define FOURGRID_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QProgressBar>
#include <QGridLayout>
#include <QTimer>
#include "widget.h"


class FourGrid : public QWidget
{
    Q_OBJECT

private:
    QFrame *frames[2][2];
    QTimer *timer;
    int currentTime;
    int totalTime;

    QPushButton *btnStart;
    QPushButton *btnStop;
    QProgressBar *progressBar;

    Widget *w1;
    Widget *w2;

    QLabel *label1;
    QLabel *label2;
    QLabel *label3;
    QLabel *label4;

    void setupUI();
    void initSignalSlots();
    static void addContentToFrame(QFrame *frame, QWidget *widget);
    void addContentToFrame(QWidget *widget, int row, int col);


    virtual void resizeEvent(QResizeEvent *event) override;

public:
    explicit FourGrid(QWidget *parent = nullptr);
    ~FourGrid() override;

public slots:
    void onTimeChanged();
};
#endif // FOURGRID_H
