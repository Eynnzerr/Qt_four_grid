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
#include "fourgrid.h"
#include "Configuration.h"

class Welcome: public QWidget
{
    Q_OBJECT
private:
    QLabel *title;
    QLabel *subtitle;
    QPushButton *btnNew;
    QPushButton *btnHistory;

    // TODO 抽出一个Widget基类，使用模版模式
    void setupUI();
    void initSignalSlots();

public:
    explicit Welcome(QWidget *parent = nullptr);
    ~Welcome() override;

public slots:
};

#endif //FOUR_GRID_WELCOME_H
