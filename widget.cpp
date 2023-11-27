#include "widget.h"
#include <QPen>
#include <QTime>
#include <iostream>
#include <qdebug.h>
#include <sstream>
// #include "json.hpp"

#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

#include <QPen>
#include <QTime>
#include <iostream>

#include <algorithm>
#include <math.h>

#include <set>



Widget::Widget(QWidget *parent) : QWidget(parent) {
    this->init_widget();
}

Widget::Widget(int topo, int packet, char *configPath, QWidget *parent): QWidget(parent) {
    qDebug() << "topo = " << topo << ", packet = " << packet;
    enable_topo   = topo;
    enable_packet = packet;
    enable_satelite = 1;
    this->configPath = configPath;

    this->init_widget();
}

Widget::Widget(int topo, int packet, QWidget *parent): QWidget(parent) {
    qDebug() << "topo = " << topo << ", packet = " << packet;
    enable_topo   = topo;
    enable_packet = packet;
    enable_satelite = 1;
    this->configPath = "/home/hylester/ns3/ns-allinone-3.38/ns-3.38/A-FINISH-2d-plot.json";
    this->init_widget();
}
Widget::~Widget() {}

void Widget::init_widget () {
// 初始化数据
    packet_line_color[0][0] = 0; packet_line_color[0][1] = 0; packet_line_color[0][2] = 0; // CBR
    packet_line_color[1][0] = 0; packet_line_color[1][1] = 0; packet_line_color[1][2] = 255; // VOIP
    packet_line_color[2][0] = 0; packet_line_color[2][1] = 255; packet_line_color[2][2] = 0; // HTTP
    packet_line_color[3][0] = 255; packet_line_color[3][1] = 0; packet_line_color[3][2] = 0; // CSXMP

    topo_link_color[0][0] = 255; topo_link_color[0][1] = 0; topo_link_color[0][2] = 128; // 水
    topo_link_color[1][0] = 0; topo_link_color[1][1] = 128; topo_link_color[1][2] = 255; // wifi
    topo_link_color[2][0] = 128; topo_link_color[2][1] = 255; topo_link_color[2][2] = 0; // 光

    // double node_pos[100][5];
    for (int i = 0; i < 100; i++) {
        for (int j = 0; j < 5; j++) {
            node_pos[i][j] = 0;
        }
    }
    for (int i = 0; i < 100; i++) {
        for (int j = 0; j < 100; j++) {
            stream_type_matrix[i][j] = 0;
        }
    }

    x_coefficient = 1.0*cur_width/default_width;
    y_coefficient = 1.0*cur_height/default_height;
    qDebug() << "create Widget!" << " x_coefficient = " <<x_coefficient << ", y_coefficient = " << y_coefficient;
    // this->resize(2000, 2000);

//    btn1 = new QPushButton("Start", this);
//    btn2 = new QPushButton("Stop", this);
//    btn3 = new QPushButton("Cut Show", this);
//    lineEdit = new QLineEdit(this);
//
//    label1 = new QLabel(this);
//    label2 = new QLabel(this);
//    // output = new QTextEdit(this);
//    label1->setGeometry(360, 10, 50, 30);
//    label1->setText("Frame:");
//    label2->setGeometry(420, 10, 40, 30);
//
//    btn1->setGeometry(10, 10, 80, 30);
//    btn2->setGeometry(100, 10, 80, 30);
//    lineEdit->setGeometry(190, 10, 80, 30);
//    btn3->setGeometry(280, 10, 80, 30);
//
//    label2->setText("0");

    // output->setGeometry(100, 770, 500, 75);

    canvas_origin[0][0] = 100;
    canvas_origin[0][1] = 250 + 200;
    canvas_origin[1][0] = 100;
    canvas_origin[1][1] = 500 + 200;
    canvas_origin[2][0] = 100;
    canvas_origin[2][1] = 750 + 200;

    canvas_height = 223;
    canvas_width = 1000;
    // 设置宽度。
    this->drawScene();

    pix_img1 = QPixmap(":/res/auv.png");
    pix_img1 = pix_img1.scaled(50, 50);
    pix_img2 = QPixmap(":/res/uav.png");
    pix_img2 = pix_img2.scaled(50, 50);
    pix_img3 = QPixmap(":/res/new_usv.png");
    pix_img3 = pix_img3.scaled(50, 50);
    pix_img4 = QPixmap(":/test.jpg");
    pix_img4 = pix_img4.scaled(50, 50);

    double node_pos[100][5];
    for (int i = 0; i < 100; i++) {
        for (int j = 0; j < 5; j++) {
            node_pos[i][j] = 0;
        }
    }

    // char *filename = "/home/hylester/ns3/ns-allinone-3.38/ns-3.38/2d-plot.json";
    // TODO: 修改
    const char *filename = configPath;
    this->parse_json(filename);

    // Create a timer to refresh the points every 20ms
//    connect(btn1, &QPushButton::clicked, [=]() {
//        timer = new QTimer(this);
//
//        //    qDebug() << allMessage.size();
//        connect(timer, &QTimer::timeout, [=]() { updatePosition(); });
//        timer->start(10);
//        connect(btn2, &QPushButton::clicked, timer, &QTimer::stop);
//    });
//    connect(btn3, &QPushButton::clicked, [=]() { showPosition(); });

}


//     Add your points here or load them from a file/database.
void Widget::parse_json(const char *filename) {

    QFile loadFile(filename);
    if (!loadFile.open(QIODevice::ReadOnly)) {
        qDebug() << "could't open projects json";
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

    QJsonObject real_rootObj = jsonDoc.object();
    QJsonObject rootObj = real_rootObj["2d_message"].toObject();
    QStringList keys = rootObj.keys();
    for (int i = 0; i < keys.size(); i++) {
        qDebug() << "key" << i << " is:" << keys.at(i);
    }

    QJsonObject aquaObj = rootObj["aqua"].toObject();
    QJsonArray aquaNodePos = aquaObj.value("Node_positon").toArray();

    QJsonObject aodvObj = rootObj["aodv"].toObject();
    QJsonArray aodvNodePos = aodvObj.value("Node_positon").toArray();

    double sim_time = aquaObj["sim_time"].toDouble();

    // 时间戳总共是 1s 100 个
    // 额外多加一些防止越界
    int frame_num = (int)(sim_time * 100) + 1000;

    std::set<int> aqua_node_index_set;
    std::set<int> aodv_node_index_set;
    std::set<int> light_node_index_set; // 额外增加光节点


    tp.points.clear();
    double min_x = 10000, min_y = 10000, max_x = -10000, max_y = -10000;
    for (int i = 0; i < aquaNodePos.size(); i++) {
        // qDebug() << i <<" value is:" << aquaNodePos.at(i);
        QJsonObject cur_pos = aquaNodePos.at(i).toObject();
        int node_id = cur_pos["node_id"].toInt();

        double x = cur_pos["position_x"].toDouble();
        double y = cur_pos["position_y"].toDouble();
        double z = cur_pos["position_z"].toDouble();

        // qDebug() << "node_id = " << node_id << ", x = "<< x << ", y = " << y;
        aqua_node_index_set.insert(node_id);

        node_pos[node_id][0] = 0;
        node_pos[node_id][1] = x;
        node_pos[node_id][2] = y;
        node_pos[node_id][3] = z;

        min_x = std::min(min_x, node_pos[node_id][1]);
        max_x = std::max(max_x, node_pos[node_id][1]);
        min_y = std::min(min_y, node_pos[node_id][2]);
        max_y = std::max(max_y, node_pos[node_id][2] + z);

        node_pos[node_id][4] = 996; // 标志位
    }

    for (int i = 0; i < aodvNodePos.size(); i++) {
        // qDebug() << i <<" value is:" << aquaNodePos.at(i);
        QJsonObject cur_pos = aodvNodePos.at(i).toObject();
        int node_id = cur_pos["node_id"].toInt();

        double x = cur_pos["position_x"].toDouble();
        double y = cur_pos["position_y"].toDouble();
        double z = cur_pos["position_z"].toDouble();

        // qDebug() << "node_id = " << node_id << ", x = " << x
        //          << ", y = " << y + z;
        aodv_node_index_set.insert(node_id);

        node_pos[node_id][0] = 0;
        // node_pos[node_id][1] = x;
        node_pos[node_id][1] = x;
        // node_pos[node_id][2] = y+z;
        node_pos[node_id][2] = y; // 仅为了方便
        node_pos[node_id][3] = z;

        min_x = std::min(min_x, node_pos[node_id][1]);
        max_x = std::max(max_x, node_pos[node_id][1]);
        min_y = std::min(min_y, node_pos[node_id][2]);
        max_y = std::max(max_y, node_pos[node_id][2]+z);

        node_pos[node_id][4] = 996; // 标志位

        // tp.points.append ({ node_pos[node_id][1],  node_pos[node_id][2],
        // -1});
    }
    
    QJsonObject DEVICE_MESSAGE = rootObj["DEVICE_MESSAGE"].toObject();
    QJsonArray USV_WITH_LIGHT = DEVICE_MESSAGE["USV_WITH_LIGHT"].toArray();
    for (int i = 0; i < USV_WITH_LIGHT.size(); i++) {
        light_node_index_set.insert(USV_WITH_LIGHT.at(i).toInt());
    }

    qDebug() << "min_x = " << min_x << ", max_x = " << max_x
             << "min_y = " << min_y << ", max_y = " << max_y;



    QJsonObject node_pos_series = rootObj["node_pos_series"].toObject();
    // json 文件里的 时间单位是 ns, 就是 10^-9 s. 换算成帧的话需要 * 0.0000001
    // 直接把后面 7 位 截除。
    QStringList node_id_message_line = node_pos_series.keys();


    // node_pos[i] , 分别为： 节点种类， 节点x, 节点y, 节点z, 该节点是否真的存在
    // double node_pos[100][5];
    // 遍历上述数组。
    for (int i = 0; i < 100; i++) {
        if (node_pos[i][4] != 996) {
            continue;
        }
        int node_type_index;

        // 当年自己给自己挖的坑: 海平面不是从0开始的
        // 海平面是 40
        int node_size = 50; // 这个是画图的节点大小
        if (node_pos[i][3] < 38) {
            node_type_index = 1;
        } else if (node_pos[i][3] > 42) {
            node_type_index = 2;
            node_size = 20;
        } else {

            node_type_index = 3;
        }
        // qDebug() << "x_source = " << x_source << ", y_source = " << y_source;
        tp.points.append({node_pos[i][1], node_pos[i][2], node_pos[i][3],
                          node_type_index, node_size, node_size});
    }

    allMessage.append(tp); // 第 0 帧
    qDebug() << "2023:10:20 :::tp.points size = " << tp.points.size();
    for (int i = 0; i < tp.points.size(); i++) {
        tp.points[i].x = 0;
        tp.points[i].y = 0;
        tp.points[i].z = 0;
    }

    // node_pos_Message
    for (int i = 1; i < frame_num + 1000; i++) { // 在这里是 构造等长数组， 全部填充0
        allMessage.append(tp);
        node_pos_Message.append(tp);
    }
    tp.points.clear();
    qDebug() << "2023:10:20 ::: node_pos_Message size = "
             << node_pos_Message.size();




    // 这里的定义放到了上面 后面发现其实不需要。
    // QJsonObject node_pos_series = rootObj["node_pos_series"].toObject();
    // QStringList node_id_message_line = node_pos_series.keys();

    for (int i = 0; i < node_id_message_line.size(); i++) {
        // 李某人的脑回路太简单了，变量命名方式简单到令人发指
        int cur_node_id_num = node_id_message_line.at(i).toInt();

        QJsonObject cur_node_id =
            node_pos_series[node_id_message_line.at(i)].toObject();
        QStringList pos_time_series = cur_node_id.keys();
        // 无需key 0， 注意这里的可能bug， 如果时间戳小于9， chop会有问题
        for (int j = 1; j < pos_time_series.size(); j++) {
            QString cur_time = pos_time_series.at(j);
            double cur_pos_x = cur_node_id[cur_time].toObject()["x"].toDouble();
            double cur_pos_y = cur_node_id[cur_time].toObject()["y"].toDouble();
            double cur_pos_z = cur_node_id[cur_time].toObject()["z"].toDouble();

            min_x = std::min(min_x, cur_pos_x);
            max_x = std::max(max_x, cur_pos_x);
            min_y = std::min(min_y, cur_pos_y);
            max_y = std::max(max_y, cur_pos_y + cur_pos_z);

            cur_time.chop(7);

            int cur_frame = cur_time.toInt();
            if (allMessage.size() > cur_frame) {
                if (allMessage[cur_frame].points.size() >= cur_node_id_num) {
                    // qDebug() << "mark";
                    // mark, 标记一下。 这里后面可能有问题
                    allMessage[cur_frame].points[cur_node_id_num - 1].x =
                        cur_pos_x;
                    allMessage[cur_frame].points[cur_node_id_num - 1].y =
                        cur_pos_y;
                    allMessage[cur_frame].points[cur_node_id_num - 1].z =
                        cur_pos_z;
                } else {
                    // 节点溢出？
                    qDebug() << "node-id = " << cur_node_id_num
                             << ", cur_frame = " << cur_frame
                             << ", x = " << cur_pos_x << ", y = " << cur_pos_y
                             << ", z = " << cur_pos_z;
                }
            } else {
                // 时间溢出？
                qDebug() << "node-id = " << cur_node_id_num
                         << ", cur_frame = " << cur_frame
                         << ", x = " << cur_pos_x << ", y = " << cur_pos_y
                         << ", z = " << cur_pos_z;
            }
        }
    }

        // 此处决定边界


    // 在这里进行插值
    // 需要注意的是， 在插值的过程中，所有的坐标都是原始坐标
    // 使用原始坐标 方便后续计算通信距离。
    const int all_node_count_xxxx = aquaObj["node_num"].toInt();
    for (int j = 0; j < all_node_count_xxxx; j++) {
        int diff = 0;
        for (int i = 1; i < allMessage.size(); i++) {
            // 如果没有值
            if (allMessage[i].points[j].x < 0.001 &&
                allMessage[i].points[j].y < 0.001) {
                diff++;
                allMessage[i].points[j].x = allMessage[i - 1].points[j].x;
                allMessage[i].points[j].y = allMessage[i - 1].points[j].y;
                allMessage[i].points[j].z = allMessage[i - 1].points[j].z;
            } else {
                // if (j == 1) {
                //     qDebug() << "frame-" << i << ", new pos, diff = " << diff;
                // }
                if (diff == 0)
                    continue;
                // qDebug() << "frame-" << i
                //          << ", new pos, x = " << allMessage[i].points[j].x
                //          << ", old pos, x= "
                //          << allMessage[i - diff - 1].points[j].x;

                double diff_x = 1.0 *
                                (allMessage[i].points[j].x -
                                 allMessage[i - diff - 1].points[j].x) /
                                (diff + 1);
                double diff_y = 1.0 *
                                (allMessage[i].points[j].y -
                                 allMessage[i - diff - 1].points[j].y) /
                                (diff + 1);
                double diff_z = 1.0 *
                                (allMessage[i].points[j].z -
                                 allMessage[i - diff - 1].points[j].z) /
                                (diff + 1);
                // qDebug() << "diff_X = " << diff_x << "diff_y = " << diff_y;
                for (int kk = 1; kk <= diff; kk++) {
                    allMessage[i - kk].points[j].x =
                        allMessage[i - kk + 1].points[j].x - diff_x;
                    allMessage[i - kk].points[j].y =
                        allMessage[i - kk + 1].points[j].y - diff_y;

                }
                if (allMessage[i-diff].points[j].x - diff_x - allMessage[i-diff-1].points[j].x > 0.01 ) {
                    qDebug() << "insert error " ;
                }
                diff = 0;
            }
        }
    }

    QJsonObject CHANNEL_MESAGE = rootObj["CHANNEL_MESAGE"].toObject();

    // TODO: 修改为从配置文件中读取
    const int aqua_range_auv = CHANNEL_MESAGE["SOUND_LEN"].toInt();
    const int aqua_range_usv = CHANNEL_MESAGE["SOUND_LEN"].toInt();
    const int aodv_range_usv = CHANNEL_MESAGE["RADIO_LEN"].toInt();
    const int aodv_range_uav = CHANNEL_MESAGE["RADIO_LEN"].toInt();
	const int light_range_auv   = CHANNEL_MESAGE["LIGHT_LEN"].toInt();
    
    int aodv_range = 5000;
    int aqua_range = 900;
    int light_range = light_range_auv;
    // std::set<int> aqua_node_index_set;
    // std::set<int> aodv_node_index_set;
    qDebug() << "aqua_node_index_set: size = " << aqua_node_index_set.size()
             << ", aodv_node_index_set: size = " << aodv_node_index_set.size();
    
    // 我们假定拓扑在1s内变化不大， 每到整s时计算所得的连线将会持续1s
    // int line_cal_time_cur = frame_num / 100;
    // node_pos_Message
    int line_cal_time_cur = (int)sim_time;
    for (int cur_second = 0; cur_second < line_cal_time_cur; cur_second++) {
        int cur_frame = cur_second * 100;

        // qDebug() << cur_frame ;
        tl.lines.clear();
        // 水声节点部分
        for (std::set<int>::iterator first = aqua_node_index_set.begin();
            first != aqua_node_index_set.end(); ++first) {
            if (allMessage[cur_frame].points[*first - 1].node_type == 1) {
                aqua_range = aqua_range_auv;
            } else {
                aqua_range = aqua_range_usv;
            }
            // aqua_range += 2000;
            std::set<int>::iterator second = first;
            second++;
            for (; second != aqua_node_index_set.end(); ++second) {
                // std::cout << *first << ", " << *second << std::endl;
                Point &aa = allMessage[cur_frame].points[*first - 1];
                Point &bb = allMessage[cur_frame].points[*second - 1];
                if ((aa.x - bb.x) * (aa.x - bb.x) +
                        (aa.y - bb.y) * (aa.y - bb.y) +
                        (aa.z - bb.z) * (aa.z - bb.z) -
                        aqua_range * aqua_range <
                    0) {
                    tl.lines.append({*first - 1, *second - 1, topo_link_color[0][0], topo_link_color[0][1], topo_link_color[0][2] });
                }
            }
        }
        aqua_line_message.append(tl);
        tl.lines.clear();
        
        // aodv节点部分
        for (std::set<int>::iterator first = aodv_node_index_set.begin();
             first != aodv_node_index_set.end(); ++first) {
            if (allMessage[cur_frame].points[*first - 1].node_type == 2) {
                aodv_range = aodv_range_uav;
            } else {
                aodv_range = aodv_range_usv;
            }

            std::set<int>::iterator second = first;
            second++;
            for (; second != aodv_node_index_set.end(); ++second) {
                // std::cout << *first << ", " << *second << std::endl;
                Point &aa = allMessage[cur_frame].points[*first - 1];
                Point &bb = allMessage[cur_frame].points[*second - 1];
                if ((aa.x - bb.x) * (aa.x - bb.x) +
                        (aa.y - bb.y) * (aa.y - bb.y) +
                        (aa.z - bb.z) * (aa.z - bb.z) -
                        aodv_range * aodv_range <
                    0) {
                    tl.lines.append({*first - 1, *second - 1, topo_link_color[1][0], topo_link_color[1][1], topo_link_color[1][2]});
                    // qDebug() << "add aodv line ";
                }
            }
        }
        aodv_line_message.append(tl);
        tl.lines.clear();
        for (std::set<int>::iterator first = light_node_index_set.begin();
             first != light_node_index_set.end(); ++first) {

            std::set<int>::iterator second = first;
            second++;
            for (; second != light_node_index_set.end(); ++second) {
                // std::cout << *first << ", " << *second << std::endl;
                Point &aa = allMessage[cur_frame].points[*first - 1];
                Point &bb = allMessage[cur_frame].points[*second - 1];
                if ((aa.x - bb.x) * (aa.x - bb.x) +
                        (aa.y - bb.y) * (aa.y - bb.y) +
                        (aa.z - bb.z) * (aa.z - bb.z) -
                        light_range * light_range <
                    0) {
                    tl.lines.append({*first - 1, *second - 1, topo_link_color[2][0], topo_link_color[2][1], topo_link_color[2][2]});
                    // qDebug() << "add light line ";

                }
            }
        }
        sound_line_message.append(tl);
    }

    max_x += 160; min_x -= 160;
    max_y += 130; min_y -= 130;
    double x_total = max_x - min_x;
    double y_total = max_y - min_y;

    // 坐标变换， 启动。
    // const int all_node_count_xxxx = aquaObj["node_num"].toInt();
    for (int j = 0; j < all_node_count_xxxx; j++) {

        double x_source;
        double y_source;
        if (allMessage[0].points[j].node_type == 1) {
            x_source = canvas_origin[2][0];
            y_source = canvas_origin[2][1];
        } else if (allMessage[0].points[j].node_type == 2) {
            x_source = canvas_origin[0][0];
            y_source = canvas_origin[0][1];
        } else if (allMessage[0].points[j].node_type == 3) {
            x_source = canvas_origin[1][0];
            y_source = canvas_origin[1][1];
        } else {
            qDebug() << "what happend in earth";
        }

        for (int i = 0; i < allMessage.size(); i++) {
            double cur_pos_x = allMessage[i].points[j].x;
            double cur_pos_y = allMessage[i].points[j].y;
            double cur_pos_z = allMessage[i].points[j].z;

            cur_pos_y += cur_pos_z;

            double x_equal = canvas_width / x_total * (cur_pos_x - min_x);
            double y_equal = canvas_height / y_total * (cur_pos_y - min_y);

            allMessage[i].points[j].x = x_source + x_equal + y_equal * 0.5;
            allMessage[i].points[j].y = y_source - 0.866 * y_equal;
        }
    }




    // 下方为发包部分的代码。
    // for (int i = 0; i < aqua_node_index_set.siz)

    // 发送要 1s, 也就是绘制100帧
    // QJsonArray aquaMessage = aquaObj.value("Message").toArray();
    // for (int i = 0; i < aquaMessage.size(); i++) {
    //     QJsonObject cur_message = aquaMessage.at(i).toObject();

    //     int node_from = cur_message["message_from"].toDouble();
    //     int node_recv = cur_message["message_receive"].toDouble();
    //     int mege_size = cur_message["message_size"].toDouble();
    //     int first_frame = cur_message["time"].toDouble() * 100;

    //     // qDebug() << "first_frame = " << first_frame << ", from = " <<
    //     // node_from << ", recv = " << node_recv;

    //     for (int j = 0; j < 100; j++) {
    //         // 这是一个点。
    //         TimePoint &timePoints_xxxx = allMessage[first_frame + j];
    //         QVector<Point> &temp_points_xxxx = timePoints_xxxx.points;

    //         // temp_points_xxxx.append( {cur_x, cur_y, 0, -1, 50, 50});
    //         temp_points_xxxx.append({node_from, node_recv, 0, 996, 50, 50});
    //     }
    // }


    QJsonArray stream_message = rootObj.value("stream").toArray();
    // 下面开始写 画包的部分
    for (int i = 0; i < stream_message.size(); i++) {
        QJsonObject cur_steam = stream_message.at(i).toObject();
        stream_type_matrix[cur_steam["from_id"].toInt()][cur_steam["end_id"].toInt()] 
        =  cur_steam["steam_type"].toInt();
    }

    // 先处理一遍所有数据包
    QJsonObject packet_type = rootObj["packet_type"].toObject();
    QStringList packet_id_x = packet_type.keys();
    for (int i = 0; i < packet_id_x.size(); i++) {
        QJsonObject cur_message = packet_type[packet_id_x.at(i)].toObject();
        packetIDtoStreamType.insert (packet_id_x.at(i), cur_message["stream_type"].toInt());
    }

    // 开始北伐 ！！！ 感觉逻辑最难盘的一部分。
    QJsonObject aodvMessage = aodvObj.value("Message").toObject();

    // json 文件里的 时间单位是 ns, 就是 10^-9 s. 换算成帧的话需要 * 0.0000001
    // 直接把后面 7+2位 截除。
    QStringList packet_id = aodvMessage.keys();

    // CP: 遍历每个包及其经过节点和时间戳，收集得到半实物仿真所需数据结构。
    for (int i = 0; i <= 3; i ++) {
        streamTimeTraces.insert(i, QVector<TimeTrace>());
    }
    for (int i = 0; i < packet_id.size(); i ++) {
        int packet_line_type = packetIDtoStreamType[packet_id.at(i)];
        QJsonObject cur_message = aodvMessage[packet_id.at(i)].toObject();
        QStringList packet_trace = cur_message.keys();
        TimeTrace timeTrace; // timeTrace delay和timestamp 单位都是10ms
        timeTrace.delay = packet_trace.last().chopped(7+2).toInt() - packet_trace.first().chopped(7+2).toInt();
        if (timeTrace.delay <= 0) {
            // 异常数据或发送未成功数据，剔除
            continue;
        }
        timeTrace.timestamp = packet_trace.first().chopped(7+2).toInt();
        for (int j = 0; j < packet_trace.size(); ++j) {
            int node = cur_message[packet_trace[j]].toInt();
            timeTrace.trace.append(node);
        }
        streamTimeTraces[packet_line_type].append(timeTrace);
    }
    for (int i = 0; i <= 3; i ++) {
        std::sort(streamTimeTraces[i].begin(), streamTimeTraces[i].end(), compareByTime);
    }
    std::cout << "半实物时延数据：\n" << getSerializedStreamTimeTraces() << std::endl;

    // 遍历每个包及其经过节点和时间戳
    for (int i = 0; i < packet_id.size(); i++) {
        QJsonObject cur_message = aodvMessage[packet_id.at(i)].toObject();
        QStringList packet_trace = cur_message.keys();

        QString frame = packet_trace.at(0);
        int pre_node = cur_message[frame].toInt(); // 始发节点id
        // 帧单位为10^-2秒，再除去末尾英文，得到帧单位下的时间戳
        frame.chop(7 + 2);
        int pre_frame = frame.toInt();
        
        int send_node = pre_node;
        QString end_frame = packet_trace.at(packet_trace.size()-1);
        int recv_node = cur_message[end_frame].toInt();
        int packet_line_type = 0;
        qDebug() << "send node = " << send_node << ", recv node = " << recv_node;
        qDebug() << "stream_type_matrix[send_node][recv_node] = " << stream_type_matrix[send_node][recv_node];
        if (packetIDtoStreamType.contains(packet_id.at(i))) {
            qDebug() << "real type = " << packetIDtoStreamType[packet_id.at(i)];
            packet_line_type = packetIDtoStreamType[packet_id.at(i)];
        } else {
            qDebug() << "packet type not record! error";
        }

        // 如果有问题的话就需要从前往后
        for (int j = 1; j < packet_trace.size(); j++) {
            QString frame = packet_trace.at(j);
            int cur_node = cur_message[frame].toInt();
            frame.chop(7 + 2);
            int cur_frame = frame.toInt();
            if (pre_frame > cur_frame) {
                qDebug() << "Error!";
            }
            // 多画半秒方便观看
            for (int temp_frame = pre_frame; temp_frame <= cur_frame + 50;
                 temp_frame++) {
                TimePoint &timePoints_xxxx = allMessage[temp_frame];
                QVector<Point> &temp_points_xxxx = timePoints_xxxx.points;
                // 画包 // 如何获得包的颜色
                temp_points_xxxx.append({pre_node, cur_node,
                        packet_line_type,                     
                        996, 50, 50});
            }
            pre_node = cur_node;
            pre_frame = cur_frame;
        }
    }
}


void Widget::updatePosition() {
    if (currentTimestamp < allMessage.size()) {
        currentTimestamp++;
    } else {
        currentTimestamp = 0;
    }
    update();
}

void Widget::setCoefficient(int cur_w, int cur_h) {

    cur_width = cur_w - 100; cur_height = cur_h - 100;

    x_coefficient = 1.0*cur_width/default_width;
    y_coefficient = 1.0*cur_height/default_height;
    this->clearScene();
    this->drawScene();
}

void Widget::clearScene() {
    parallelogram_sky.clear();

    parallelogram_land.clear();
    parallelogram_underwater.clear();
}

void Widget::drawScene () {

    // canvas_origin[0][0] = 100;
    // canvas_origin[0][1] = 250;
    // canvas_origin[1][0] = 100;
    // canvas_origin[1][1] = 500 + 200;
    // canvas_origin[2][0] = 100;
    // canvas_origin[2][1] = 750 + 200;
    
    // int default_width  = 1200;
    // int default_height = 950;

    // int canvas_height = 200;
    // int canvas_width  = 1000;

    parallelogram_sky.moveTo((canvas_origin[0][0] )*x_coefficient, ( canvas_origin[0][1] )*y_coefficient);
    parallelogram_sky.lineTo((canvas_origin[0][0]+100 )*x_coefficient, ( canvas_origin[0][1]-200 )*y_coefficient);
    parallelogram_sky.lineTo((canvas_origin[0][0]+1100 )*x_coefficient, ( canvas_origin[0][1]-200 )*y_coefficient);
    parallelogram_sky.lineTo((canvas_origin[0][0]+1000 )*x_coefficient, ( canvas_origin[0][1] )*y_coefficient);
    parallelogram_sky.lineTo((canvas_origin[0][0] )*x_coefficient, ( canvas_origin[0][1] )*y_coefficient);


    parallelogram_sky.moveTo((canvas_origin[1][0] )*x_coefficient, ( canvas_origin[1][1] )*y_coefficient);
    parallelogram_sky.lineTo((canvas_origin[1][0]+100 )*x_coefficient, ( canvas_origin[1][1]-200 )*y_coefficient);
    parallelogram_sky.lineTo((canvas_origin[1][0]+1100 )*x_coefficient, ( canvas_origin[1][1]-200 )*y_coefficient);
    parallelogram_sky.lineTo((canvas_origin[1][0]+1000 )*x_coefficient, ( canvas_origin[1][1] )*y_coefficient);
    parallelogram_sky.lineTo((canvas_origin[1][0] )*x_coefficient, ( canvas_origin[1][1] )*y_coefficient);
    
    parallelogram_sky.moveTo((canvas_origin[2][0] )*x_coefficient, ( canvas_origin[2][1] )*y_coefficient);
    parallelogram_sky.lineTo((canvas_origin[2][0]+100 )*x_coefficient, ( canvas_origin[2][1]-200 )*y_coefficient);
    parallelogram_sky.lineTo((canvas_origin[2][0]+1100 )*x_coefficient, ( canvas_origin[2][1]-200 )*y_coefficient);
    parallelogram_sky.lineTo((canvas_origin[2][0]+1000 )*x_coefficient, ( canvas_origin[2][1] )*y_coefficient);
    parallelogram_sky.lineTo((canvas_origin[2][0] )*x_coefficient, ( canvas_origin[2][1] )*y_coefficient);
    

}

void Widget::DrawLineWithArrow(QPainter &painter, QPen pen, QPoint start,
                               QPoint end, int with_arrow) {
    painter.setRenderHint(QPainter::Antialiasing, true);

    qreal arrowSize = 20;
    if (!with_arrow) { // 包， 画实线
        pen.setStyle(Qt::DotLine);
    } else { // 拓扑， 画虚线
        pen.setStyle(Qt::SolidLine);
    }
    pen.setWidth(3);
    painter.setPen(pen);
    painter.setBrush(pen.color());

    QLineF line(end, start);
    painter.drawLine(line);
    if (!with_arrow) return;

    // qDebug() << "Widget::DrawLineWithArrow, x = ";
    double angle = std::atan2(-line.dy(), line.dx());
    QPointF arrowP1 = line.p1() + QPointF(sin(angle + M_PI / 3) * arrowSize,
                                          cos(angle + M_PI / 3) * arrowSize);
    QPointF arrowP2 =
        line.p1() + QPointF(sin(angle + M_PI - M_PI / 3) * arrowSize,
                            cos(angle + M_PI - M_PI / 3) * arrowSize);

    QPolygonF arrowHead;
    arrowHead.clear();
    arrowHead << line.p1() << arrowP1 << arrowP2;
    painter.drawPolygon(arrowHead);
}

void Widget::showPosition() {
//    if (lineEdit->text() != NULL) {
//        currentTimestamp = lineEdit->text().toInt();
//    }
    update();
}

void Widget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);

    // Area paint
    painter.drawPath(parallelogram_sky);
    painter.drawPath(parallelogram_land);
    painter.drawPath(parallelogram_underwater);

    QPen pen;
    pen.setColor(QColor(0, 0, 0));
    pen.setWidth(5);
    painter.setPen(pen);
    painter.setRenderHint(QPainter::Antialiasing);

    QPen pen2 = QPen(QColor(0, 0, 255), 2, Qt::SolidLine);

    // 图例
    pen.setStyle(Qt::DotLine);
    pen.setColor(QColor(topo_link_color[1][0], topo_link_color[1][1], topo_link_color[1][2]));
    painter.setPen(pen);
    painter.drawLine(QPoint(60, 45), QPoint(90, 45));
    pen.setColor(QColor(topo_link_color[2][0], topo_link_color[2][1], topo_link_color[2][2]));
    painter.setPen(pen);
    painter.drawLine(QPoint(60, 65), QPoint(90, 65));
    pen.setColor(QColor(topo_link_color[0][0], topo_link_color[0][1], topo_link_color[0][2]));
    painter.setPen(pen);
    painter.drawLine(QPoint(60, 85), QPoint(90, 85));

    pen.setColor(QColor(0, 0, 0));
    painter.setPen(pen);
    painter.drawText(100, 50, "射频链路");
    painter.drawText(100,70, "光链路");
    painter.drawText(100, 90, "水声链路");

    pen.setStyle(Qt::SolidLine);
    pen.setColor(QColor(packet_line_color[0][0], packet_line_color[0][1], packet_line_color[0][2]));
    painter.setPen(pen);
    painter.drawLine(QPoint(200, 25), QPoint(230, 25));
    pen.setColor(QColor(packet_line_color[1][0], packet_line_color[1][1], packet_line_color[1][2]));
    painter.setPen(pen);
    painter.drawLine(QPoint(200, 45), QPoint(230, 45));
    pen.setColor(QColor(packet_line_color[2][0], packet_line_color[2][1], packet_line_color[2][2]));
    painter.setPen(pen);
    painter.drawLine(QPoint(200, 65), QPoint(230, 65));
    pen.setColor(QColor(packet_line_color[3][0], packet_line_color[3][1], packet_line_color[3][2]));
    painter.setPen(pen);
    painter.drawLine(QPoint(200, 85), QPoint(230, 85));

    pen.setColor(QColor(0, 0, 0));
    painter.setPen(pen);
    painter.drawText(240, 30, "CBR流");
    painter.drawText(240,50, "VoIP流");
    painter.drawText(240, 70, "HTTP流");
    painter.drawText(240, 90, "CSXMP流");

    if (enable_topo)
    { // 下面部分是绘制发包的
        int cur_second = currentTimestamp / 100;
        if (cur_second >= aqua_line_message.size())
        {
            return;
        }
        else
        {
            lines_aodv = aodv_line_message[cur_second];
            lines_aqua = aqua_line_message[cur_second];
            lines_light = sound_line_message[cur_second];
        }
        const TimePoint &timePoints = allMessage[currentTimestamp];
        const QVector<Point> &points = timePoints.points;
        // pen2.setOpacity(1); // 设置透明度为50%
        int trans_rate = 178;
        pen2.setWidth(4);
        for (const Line &line : lines_aqua.lines)
        {
            int first = line.begin_id;
            int second = line.end_id;
            int x1 = points[first].x * x_coefficient;
            int y1 = points[first].y * y_coefficient;
            int x2 = points[second].x * x_coefficient;
            int y2 = points[second].y * y_coefficient;
            pen2.setColor(QColor(line.color_r, line.color_g, line.color_b, trans_rate));
            // pen2.setColor(QColor(line.color_r, line.color_g, line.color_b, (line.color_r+ line.color_g+line.color_b)/3));
            DrawLineWithArrow(painter, pen2, QPoint(x1, y1), QPoint(x2, y2), 0);
        }

        pen2.setWidth(2); // 先画的粗一点， 被覆盖了可以看到
        for (const Line &line : lines_light.lines)
        {
            int first = line.begin_id;
            int second = line.end_id;
            int x1 = points[first].x * x_coefficient;
            int y1 = points[first].y * y_coefficient;
            int x2 = points[second].x * x_coefficient;
            int y2 = points[second].y * y_coefficient;
            pen2.setColor(QColor(line.color_r, line.color_g, line.color_b, trans_rate));
            // pen2.setColor(QColor(line.color_r, line.color_g, line.color_b, (line.color_r+ line.color_g+line.color_b)/3));
            DrawLineWithArrow(painter, pen2, QPoint(x1, y1), QPoint(x2, y2), 0);
        }
        pen2.setWidth(2);
        for (const Line &line : lines_aodv.lines)
        {
            int first = line.begin_id;
            int second = line.end_id;
            int x1 = points[first].x * x_coefficient;
            int y1 = points[first].y * y_coefficient;
            int x2 = points[second].x * x_coefficient;
            int y2 = points[second].y * y_coefficient;
            pen2.setColor(QColor(line.color_r, line.color_g, line.color_b, trans_rate));
            // pen2.setColor(QColor(line.color_r, line.color_g, line.color_b, (line.color_r+ line.color_g+line.color_b)/3));
            DrawLineWithArrow(painter, pen2, QPoint(x1, y1), QPoint(x2, y2), 0);
        }
    }

    if (!allMessage.empty()) {
        const TimePoint &timePoints = allMessage[currentTimestamp];
        const QVector<Point> &points = timePoints.points;
        // int id = 1;
        for (const Point &point : points) {
            double x = point.x*x_coefficient;
            double y = point.y*y_coefficient;
            if (point.node_type == 1) {
                painter.drawPixmap(
                    x - point.node_width * 0.5, y - point.node_height * 0.5,
                    point.node_width, point.node_height, pix_img1);
                // painter.drawText(x - point.node_width * 0.5, y, QString::number(id));
            } else if (point.node_type == 2) {
                painter.drawPixmap(
                    x - point.node_width * 0.5, y - point.node_height * 0.5,
                    point.node_width, point.node_height, pix_img2);
                // painter.drawText(x - point.node_width * 0.5, y, QString::number(id));
            } else if (point.node_type == 3) {
                painter.drawPixmap(
                    x - point.node_width * 0.5, y - point.node_height * 0.5,
                    point.node_width, point.node_height, pix_img3);
                // painter.drawText(x - point.node_width * 0.5, y, QString::number(id));
            } else if (point.node_type == 996) {
                // 画包路径
                if (!enable_packet) continue;
                int first = (int)point.x;
                int second = (int)point.y;
                if (first == second) {
                    continue;
                }
                int colorType = (int)point.z;
                if (colorType != 0)
                    pen2.setColor(QColor(packet_line_color[colorType][0],
                                         packet_line_color[colorType][1], packet_line_color[colorType][2]
                                         // ,255
                                         // (packet_line_color[colorType][0]+packet_line_color[colorType][1]+packet_line_color[colorType][2])/2*3
                                         ));
                else
                {
                    pen2.setColor(QColor(packet_line_color[colorType][0],
                                         packet_line_color[colorType][1], packet_line_color[colorType][2]));
                }
                // 这里能运行也是巧合
                int x1 = points[first - 1].x *x_coefficient;
                int y1 = points[first - 1].y *y_coefficient;
                int x2 = points[second - 1].x *x_coefficient;
                int y2 = points[second - 1].y *y_coefficient;
                // pen2.setColor(QColor(line.color_r, line.color_g,
                // line.color_b));
                DrawLineWithArrow(painter, pen2, QPoint(x1, y1),
                                  QPoint(x2, y2), 1);
                
            } else if (point.node_type == 997) {
                int first = (int)point.x;
                int second = (int)point.y;
                if (first == second) {
                    continue;
                }
                int colorType = (int)point.z;
                pen2.setColor(QColor(packet_line_color[colorType][0],
                                     packet_line_color[colorType][1], packet_line_color[colorType][2]
                ));
                int x1 = points[first - 1].x *x_coefficient;
                int y1 = points[first - 1].y *y_coefficient;
                int x2 = points[second - 1].x *x_coefficient;
                int y2 = points[second - 1].y *y_coefficient;

                DrawLineWithArrow(painter, pen2, QPoint(x1, y1),
                                  QPoint(x2, y2), 1);
                qDebug() << "draw 997 line!";
            } else {
                qDebug() << "points.size() = " << points.size();
                // painter.drawPixmap(x-point.node_width*0.5,
                // y-point.node_height*0.5, point.node_width, point.node_height,
                // pix_img4);
            }
            // id ++;
        }
    }
}

int Widget::getTotalTime() {
    return aqua_line_message.size();
}

void Widget::resetTimestamp() {
    currentTimestamp = 0;
}

int Widget::getCurrentTime() {
    return currentTimestamp; // 单位 10ms
}

void Widget::drawLineForStreamAtTime(int stream_type, int frame) {
    for (auto timeTrace : streamTimeTraces[stream_type]) {
        if (timeTrace.timestamp >= frame) {
            // 找到了当前时间点匹配的路径
            qDebug() << "receive real stream of type " << stream_type << " at frame " << frame << "! delay: " << timeTrace.delay << " frame.";
            QPainter painter(this);
            QPen pen = QPen(QColor(packet_line_color[stream_type][0],packet_line_color[stream_type][1], packet_line_color[stream_type][2]), 2, Qt::SolidLine);

            const TimePoint &timePoints = allMessage[currentTimestamp];
            const QVector<Point> &points = timePoints.points;
            for (int i = 0; i < timeTrace.trace.size() - 1; ++i) {
                int first = timeTrace.trace[i];
                int second = timeTrace.trace[i+1];
                int x1 = points[first - 1].x * x_coefficient;
                int y1 = points[first - 1].y * y_coefficient;
                int x2 = points[second - 1].x * x_coefficient;
                int y2 = points[second - 1].y * y_coefficient;
                DrawLineWithArrow(painter, pen, QPoint(x1, y1),
                                  QPoint(x2, y2), 1);
            }
        }
    }
}

void Widget::addStreamToDisplay(int stream_type) {
    for (auto timeTrace : streamTimeTraces[stream_type]) {
        if (timeTrace.timestamp >= currentTimestamp) {
            // 找到了当前时间点匹配的路径。这里已经排序好，故第一个找到大于等于的就是最近匹配的
            qDebug() << "receive real stream of type " << stream_type << " at frame " << currentTimestamp << "! delay: " << timeTrace.delay << " frame.";
            for (int i = 0; i < timeTrace.trace.size() - 1; ++i) {
                // 逐段设置显示
                int pre_node = timeTrace.trace[i];
                int cur_node = timeTrace.trace[i+1];
                for (int temp_frame = currentTimestamp; temp_frame <= currentTimestamp + 50;
                     temp_frame++) {
                    TimePoint &timePoints_xxxx = allMessage[temp_frame];
                    QVector<Point> &temp_points_xxxx = timePoints_xxxx.points;
                    temp_points_xxxx.append({static_cast<double>(pre_node), static_cast<double>(cur_node),
                                             static_cast<double>(stream_type),
                                             997, 50, 50});

                }
            }
            break;
        }
    }
}

std::string Widget::getSerializedStreamTimeTraces() {
    QJsonObject mainObj;
    QJsonObject timeObj;
    for (auto it = streamTimeTraces.begin(); it != streamTimeTraces.end(); ++it) {
        QJsonArray timeTraceArray;
        for (const TimeTrace &timeTrace : it.value()) {
            timeTraceArray.append(timeTrace.toJsonExcludeTrace());
        }
        // mainObj[QString::number(it.key())] = timeTraceArray;
        timeObj[QString::number(it.key())] = timeTraceArray;
    }
    mainObj["code"] = 0;
    mainObj["data"] = timeObj;
    QJsonDocument doc(mainObj);
    return doc.toJson().toStdString();
}

bool Widget::compareByTime(Widget::TimeTrace t1, Widget::TimeTrace t2) {
    return t1.timestamp < t2.timestamp;
}

double Widget::getCurrentTimeBySec() {
    return getCurrentTime() / 100.0;
}


