#ifndef CHARTWIDGET_H
#define CHARTWIDGET_H

#include <QDockWidget>
#include <QString>
#include <QWidget>
#include <vector>
#include <QDebug>
#include <QtCharts>
#include <QVBoxLayout>
#include <QPushButton>
#include <QComboBox>

class ChartWidget : public QWidget {
  Q_OBJECT

private:
    char *tracePath;
    int currentTimestamp = 0;
    int currentIndex = 0;
    std::vector<QChart *> chartData;
    QStringList chartOptions;
    QComboBox *chartBox;
    QChartView *chartView;

    void parse_json (const char *filename);
    void init_chart();
public:
    explicit ChartWidget(QChart *parent = nullptr);
    explicit ChartWidget(char *tracePath, QChart *parent = nullptr);
    ~ChartWidget() override;

};
#endif
