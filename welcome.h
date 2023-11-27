//
// Created by eynnzerr on 23-10-30.
//

#ifndef FOUR_GRID_WELCOME_H
#define FOUR_GRID_WELCOME_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFile>
#include <QFileDialog>
#include <QRadioButton>
#include "fourgrid.h"
#include "Configuration.h"
#include "RealSimPage.h"

class Welcome: public QWidget
{
    Q_OBJECT
private:
    QLabel *title;
    QLabel *subtitle;
    QPushButton *btnNew;
    QPushButton *btnHistory;
    QPushButton *btnRealSim;

    QDialog *dialog;
    QButtonGroup *buttonGroup;

    // TODO 抽出一个Widget基类，使用模版模式
    void setupUI();
    void initSignalSlots();
    void setupDialog();

public:
    explicit Welcome(QWidget *parent = nullptr);
    ~Welcome() override;

public slots:
};

#endif //FOUR_GRID_WELCOME_H
