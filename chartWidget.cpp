#include "chartWidget.h"

ChartWidget::ChartWidget(QChart *parent) {
    init_chart();
}

ChartWidget::ChartWidget(char *tracePath, QChart *parent) {
    this->tracePath = tracePath;
    init_chart();
}

ChartWidget::~ChartWidget() = default;

// 爆int了， 本来想着用字符串处理时间的， 结果用double居然不爆， 多一事不如少一事
struct time_process
{
    time_process (QString x);
    int get_frame (); // 根据外部修改。
    QString time_str; // 默认
};
time_process::time_process (QString x) {
    time_str = x;
}
int time_process::get_frame () {
    if (time_str.length() <= 7) {
        return 0;
    }
    int res = 0;
    return res;
}

void ChartWidget::parse_json(const char *filename) {
    qDebug() << "chart message process";
    QFile loadFile(filename);
    if (!loadFile.open(QIODevice::ReadOnly)) {
        qDebug() << "couldn't open projects json";
        return;
    }

    QByteArray allData = loadFile.readAll();
    loadFile.close();

    QJsonParseError jsonError;
    QJsonDocument jsonDoc(QJsonDocument::fromJson(allData, &jsonError));

    if (jsonError.error != QJsonParseError::NoError) {
        qDebug() << "json error!" << jsonError.errorString();
        return;
    }

    QJsonObject rootObj = jsonDoc.object();
    QStringList keys = rootObj.keys();

    QJsonArray events =  rootObj["events"].toArray(); 
    int max_id = 0; int event_num = 0; long max_time = 0; int error_num = 0;
    QString default_str = "xy-series-append";
    QString time;
    std::vector<std::vector<std::pair<double, double>>> chartSeries;
    for (int i = 0; i < events.size(); i++) {
        QJsonObject cur_event = events.at(i).toObject();

        if (QString::localeAwareCompare(default_str, cur_event["type"].toString()) == 0) {
            event_num++;
            max_id = std::max(max_id, cur_event["series-id"].toInt());
            max_time = std::max(max_time, (long)cur_event["nanoseconds"].toDouble());

            int id = cur_event["series-id"].toInt();
            double x = cur_event["x"].toDouble();
            double y = cur_event["y"].toDouble();
            if (chartSeries.size() <= id-1) chartSeries.emplace_back();
            chartSeries[id-1].emplace_back(x, y); // 实际序号从1开始，而vector索引从0开始
        } else {
            qDebug() << "ChartWidget::parse_json error !!!"; error_num++;
            qDebug() << cur_event["type"].toString();
        }
    }
    qDebug() << "max_id = " << max_id << ", event_num = " << event_num
    << ", max_time = " << max_time << "ns" << ", error_num = " << error_num;

    QJsonArray series =  rootObj["series"].toArray(); 
    QString type1_str = "xy-series";
    QString type2_str = "series-collection";
    QString type1_label = "hidden";

    // std::vector<QChart *> chartData;
    for (int i = 0; i < series.size(); i++) {
        QJsonObject ser = series.at(i).toObject();
        // 这种是单条线
        chartOptions << ser["name"].toString();
        if (QString::localeAwareCompare(type1_str, ser["type"].toString()) == 0) {
            if (QString::localeAwareCompare(type1_label, ser["labels"].toString()) == 0) {
                qDebug() << "draw single line"  << ", type = " << ser["type"].toString();
                int idx = ser["id"].toInt() - 1;
                auto lineseries = new QLineSeries;
                for (auto it : chartSeries[idx]) {
                    lineseries->append(it.first, it.second);
                }
                auto chart = new QChart;
                chart->addSeries(lineseries);
                chart->createDefaultAxes();
                chart->axes().first()->setTitleText("时间(s)");
                chart->axes().last()->setTitleText("吞吐量(Mbps)");
                chartData.push_back(chart);
            }
        } 
        // 这种是多条线段
        else if (QString::localeAwareCompare(type2_str, ser["type"].toString()) == 0) {
            qDebug() << "id = " << ser["id"].toInt() 
                     << ", name = " << ser["name"].toString()
                     << "child_num = " << ser["child-series"].toArray().size();
            QJsonArray indexes =  ser["child-series"].toArray();
            auto chart = new QChart;
            for (auto && index : indexes) {
                int idx = index.toInt() - 1;
                auto lineseries = new QLineSeries;
                for (auto it : chartSeries[idx]) {
                    lineseries->append(it.first, it.second);
                    lineseries->setColor(QColor::fromHsl(rand()%360,rand()%256,rand()%200));
                }
                chart->addSeries(lineseries);
            }
            chart->createDefaultAxes();
            chart->axes().first()->setTitleText("时间(s)");
            chart->axes().last()->setTitleText("吞吐量(Mbps)");
            chartData.push_back(chart);
        } else {
            qDebug() << "ChartWidget::parse_json error !!!";
            qDebug() << ser["type"].toString();
        }
    }
}

void ChartWidget::init_chart() {
    // char* filename = "/home/eynnzerr/open/JetBrainsWorkSpace/CLion/ns-allinone-3.38/ns-3.38/aaa--finish.json";
    // this->parse_json(filename);
    this->parse_json(tracePath);

    chartView = new QChartView(chartData[0]);
    chartView->setRenderHint(QPainter::Antialiasing); // 抗锯齿
    chartView->chart()->legend()->hide();

    auto btnSwitch = new QPushButton("下一个");
    connect(btnSwitch, &QPushButton::clicked, [=] {
        chartView->setChart(chartData[++currentIndex]);
        chartBox->setCurrentIndex(currentIndex);
    });

    chartBox = new QComboBox;
    chartBox->addItems(chartOptions);
    // chartBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(chartBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [=](int index) {
        chartView->setChart(chartData[index]);
        chartView->chart()->legend()->hide();
        currentIndex = index;
    });

//    auto hLayout = new QHBoxLayout;
//    hLayout->addWidget(chartBox);
//    hLayout->addWidget(btnSwitch);

    auto layout = new QVBoxLayout;
    layout->addWidget(chartBox);
    layout->addWidget(chartView);
    setLayout(layout);

//     auto axisX = new QValueAxis;
//     auto axisY = new QValueAxis;
//     axisX->setRange(0,10);
//     axisX->setLabelFormat("%.1f");
//     axisX->setTickCount(11);
//     axisX->setMinorTickCount(4);
//     axisX->setTitleText("time(secs)");
//
//     axisY->setRange(-1.5,1.5);
//     axisY->setTickCount(3);
//     axisY->setMinorTickCount(4);
//     axisY->setTitleText("value");
//
//     this->setAxisX(axisX,series0);
//     this->setAxisX(axisX,series1);
//     this->setAxisY(axisY,series0);
//     this->setAxisY(axisY,series1);

}
