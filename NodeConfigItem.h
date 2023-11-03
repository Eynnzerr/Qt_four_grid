//
// Created by eynnzerr on 23-10-31.
//

#ifndef FOUR_GRID_NODECONFIGITEM_H
#define FOUR_GRID_NODECONFIGITEM_H

#include <QWidget>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QRegExp>
#include <QRegExpValidator>
#include <QJsonObject>

class NodeConfigItem: public QWidget {
    Q_OBJECT
private:
    int index;
    QComboBox *nodeTypeBox;
    QComboBox *moveMode;
    QLineEdit *moveVelocity;
    QLineEdit *positionX;
    QLineEdit *positionY;
    QLineEdit *positionZ;

    void setupUI();
    void initSignalSlots();

public:
    explicit NodeConfigItem(int index);
    int getNodeType();
    int getMoveMode();
    double getVelocity();
    double getPositionX();
    double getPositionY();
    double getPositionZ();
    QJsonObject toJsonObject();
};


#endif //FOUR_GRID_NODECONFIGITEM_H
