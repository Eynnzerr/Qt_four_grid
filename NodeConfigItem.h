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

class NodeConfigItem: public QWidget {
    Q_OBJECT
private:
    int index;
    QComboBox *moveMode;
    QLineEdit *moveVelocity;

    void setupUI();
    void initSignalSlots();

public:
    explicit NodeConfigItem(int index);
};


#endif //FOUR_GRID_NODECONFIGITEM_H
