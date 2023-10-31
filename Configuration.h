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
#include <QRegularExpression>
#include "NodeConfigItem.h"
#include "welcome.h"

class Configuration: public QWidget {
    Q_OBJECT
private:
    QLineEdit *uavNodeNumInput;
    QPushButton *uavNodeConfirm;
    QListWidget *uavConfigList;

    QLineEdit *usvNodeNumInput;
    QPushButton *usvNodeConfirm;
    QListWidget *usvConfigList;

    QLineEdit *auvNodeNumInput;
    QPushButton *auvNodeConfirm;
    QListWidget *auvConfigList;

    QPushButton *btnToSimulation;
    QPushButton *btnGoBack;

    void setupUI();
    void initSignalSlots();
public:
    explicit Configuration(QWidget *parent = nullptr);
    ~Configuration() override;
};


#endif //FOUR_GRID_CONFIGURATION_H
