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

    QJsonObject rootObj = jsonDoc.object();
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
        min_y = std::min(min_y, node_pos[node_id][2] + z);
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

        qDebug() << "node_id = " << node_id << ", x = " << x
                 << ", y = " << y + z;
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
        max_y = std::max(max_y, node_pos[node_id][2]);

        node_pos[node_id][4] = 996; // 标志位

        // tp.points.append ({ node_pos[node_id][1],  node_pos[node_id][2],
        // -1});
    }

    qDebug() << "min_x = " << min_x << ", max_x = " << max_x
             << "min_y = " << min_y << ", max_y = " << max_y;

    // 后续修改比例尺就这里, 废弃了
    double x_total = max_x - min_x;
    double y_total = max_y - min_y;

    // node_pos[i] , 分别为： 节点种类， 节点x, 节点y, 节点z, 该节点是否真的存在
    // double node_pos[100][5];
    // 遍历上述数组。
    for (int i = 0; i < 100; i++) {
        if (node_pos[i][4] < 100) {
            continue;
        }

        // 无需优化性能， 浮点数除法就懒得转换了
        double x_equal = canvas_width / x_total * (node_pos[i][1] - min_x);
        double y_equal = canvas_height / y_total * (node_pos[i][2] - min_y);
        double x_source;
        double y_source;
        int node_type_index;

        // 当年自己给自己挖的坑: 海平面不是从0开始的
        // 海平面是 40
        int node_size = 50;
        if (node_pos[i][3] < 38) {
            x_source = canvas_origin[2][0];
            y_source = canvas_origin[2][1];
            qDebug() << "sea";
            node_type_index = 1;
        } else if (node_pos[i][3] > 42) {
            x_source = canvas_origin[0][0];
            y_source = canvas_origin[0][1];
            node_type_index = 2;
            node_size = 20;
        } else {
            x_source = canvas_origin[1][0];
            y_source = canvas_origin[1][1];
            node_type_index = 3;
        }
        // qDebug() << "x_source = " << x_source << ", y_source = " << y_source;
        tp.points.append({node_pos[i][1], node_pos[i][2], node_pos[i][3],
                          node_type_index, node_size, node_size});

        node_pos[i][1] = x_source + x_equal + y_equal * 0.5;
        node_pos[i][2] = y_source - 0.866 * y_equal;
    }

    allMessage.append(tp);
    qDebug() << "2023:10:20 :::tp.points size = " << tp.points.size();
    for (int i = 0; i < tp.points.size(); i++) {
        tp.points[i].x = 0;
        tp.points[i].y = 0;
    }

    // node_pos_Message
    for (int i = 1; i < frame_num + 1000; i++) {
        allMessage.append(tp);
        node_pos_Message.append(tp);
    }
    tp.points.clear();
    qDebug() << "2023:10:20 ::: node_pos_Message size = "
             << node_pos_Message.size();

    QJsonObject node_pos_series = rootObj["node_pos_series"].toObject();

    // json 文件里的 时间单位是 ns, 就是 10^-9 s. 换算成帧的话需要 * 0.0000001
    // 直接把后面 7 位 截除。
    QStringList node_id_message_line = node_pos_series.keys();

    for (int i = 0; i < node_id_message_line.size(); i++) {
        // 李某人的脑回路太简单了，变量命名方式简单到令人发指
        int cur_node_id_num = node_id_message_line.at(i).toInt();
        double x_source;
        double y_source;

        if (allMessage[0].points[cur_node_id_num - 1].node_type == 1) {
            x_source = canvas_origin[2][0];
            y_source = canvas_origin[2][1];
        } else if (allMessage[0].points[cur_node_id_num - 1].node_type == 2) {
            x_source = canvas_origin[0][0];
            y_source = canvas_origin[0][1];
        } else if (allMessage[0].points[cur_node_id_num - 1].node_type == 3) {
            x_source = canvas_origin[1][0];
            y_source = canvas_origin[1][1];
        } else {
            qDebug() << "what happend in earth";
        }

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
            min_y = std::min(min_y, cur_pos_y + cur_pos_z);
            max_y = std::max(max_y, cur_pos_y + cur_pos_z);

            cur_pos_y += cur_pos_z;
            cur_time.chop(7);
            // qDebug() << "chop cal, cur_time = " << cur_time << ", size = " <<
            // cur_time.length();

            int cur_frame = cur_time.toInt();
            qDebug() << "node-id = " << cur_node_id_num
                     << ", cur_frame = " << cur_frame << ", x = " << cur_pos_x
                     << ", y = " << cur_pos_y << ", z = " << cur_pos_z;

            // 坐标变换启动
            double x_equal = canvas_width / x_total * (cur_pos_x - min_x);
            double y_equal = canvas_height / y_total * (cur_pos_y - min_y);

            if (allMessage.size() > cur_frame) {
                if (allMessage[cur_frame].points.size() >= cur_node_id_num) {
                    qDebug() << "mark";
                    // mark, 标记一下。
                    allMessage[cur_frame].points[cur_node_id_num - 1].x =
                        cur_pos_x;
                    allMessage[cur_frame].points[cur_node_id_num - 1].y =
                        cur_pos_y;
                    allMessage[cur_frame].points[cur_node_id_num - 1].z =
                        cur_pos_z;
                    // allMessage[cur_frame].points[cur_node_id_num-1].x =
                    // x_source + x_equal + y_equal * 0.5;
                    // allMessage[cur_frame].points[cur_node_id_num-1].y =
                    // y_source - 0.866 * y_equal;

                } else {
                    qDebug() << "node-id = " << cur_node_id_num
                             << ", cur_frame = " << cur_frame
                             << ", x = " << cur_pos_x << ", y = " << cur_pos_y
                             << ", z = " << cur_pos_z;
                }
            } else {
                qDebug() << "node-id = " << cur_node_id_num
                         << ", cur_frame = " << cur_frame
                         << ", x = " << cur_pos_x << ", y = " << cur_pos_y
                         << ", z = " << cur_pos_z;
            }
        }
        // QString frame = packet_trace.at(0);
        // int pre_node = cur_message[frame].toInt();
        // frame.chop(7+2);
        // int pre_frame = frame.toInt();
    }

    // 在这里进行插值
    const int all_node_count_xxxx = aquaObj["node_num"].toInt();
    for (int j = 0; j < all_node_count_xxxx; j++) {
        int diff = 0;
        for (int i = 1; i < allMessage.size(); i++) {

            if (allMessage[i].points[j].x < 0.001 &&
                allMessage[i].points[j].y < 0.001) {
                diff++;
                allMessage[i].points[j].x = allMessage[i - 1].points[j].x;
                allMessage[i].points[j].y = allMessage[i - 1].points[j].y;
            } else {

                if (j == 1) {
                    qDebug() << "frame-" << i << ", new pos, diff = " << diff;
                }
                if (diff == 0)
                    continue;
                qDebug() << "frame-" << i
                         << ", new pos, x = " << allMessage[i].points[j].x
                         << ", old pos, x= "
                         << allMessage[i - diff - 1].points[j].x;

                double diff_x = 1.0 *
                                (allMessage[i].points[j].x -
                                 allMessage[i - diff - 1].points[j].x) /
                                (diff + 1);
                double diff_y = 1.0 *
                                (allMessage[i].points[j].y -
                                 allMessage[i - diff - 1].points[j].y) /
                                (diff + 1);
                qDebug() << "diff_X = " << diff_x << "diff_y = " << diff_y;
                for (int kk = 1; kk <= diff; kk++) {
                    allMessage[i - kk].points[j].x =
                        allMessage[i - kk + 1].points[j].x - diff_x;
                    allMessage[i - kk].points[j].y =
                        allMessage[i - kk + 1].points[j].y - diff_y;
                    if (j == 1) {
                        // qDebug() << "frame-" << i-kk << ", changed" ;
                    }
                }
                diff = 0;
            }
        }
    }

    for (int i = 0; i < 2000; i++) {

        // qDebug() << "frame-" << i << ", x = "  << allMessage[i].points[1].x
        // <<", y = " <<allMessage[i].points[1].y ;
    }

    // for (int i = 0; i < 200; i++) {
    //     qDebug() << "node-9, frame-" << i << ": x = " <<
    //     allMessage[i].points[8].x << ", y =" <<  allMessage[i].points[8].y;
    // }

    const int aqua_range_auv = 900;
    const int aqua_range_usv = 0;
    const int aodv_range_usv = 200;
    const int aodv_range_uav = 100;
    int aodv_range = 150;
    int aqua_range = 900;

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
                    tl.lines.append({*first - 1, *second - 1, 0, 0, 255});
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
                    tl.lines.append({*first - 1, *second - 1, 255, 0, 0});
                }
            }
        }
        aodv_line_message.append(tl);
    }

    min_x -= 10;
    max_x += 10;
    min_y -= 10;
    max_y += 10;
    x_total = max_x - min_x;
    y_total = max_y - min_y;

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

    // for (int i = 0; i < aqua_node_index_set.siz)

    // 发送要 1s, 也就是绘制100帧
    QJsonArray aquaMessage = aquaObj.value("Message").toArray();
    for (int i = 0; i < aquaMessage.size(); i++) {
        QJsonObject cur_message = aquaMessage.at(i).toObject();

        int node_from = cur_message["message_from"].toDouble();
        int node_recv = cur_message["message_receive"].toDouble();
        int mege_size = cur_message["message_size"].toDouble();
        int first_frame = cur_message["time"].toDouble() * 100;

        // qDebug() << "first_frame = " << first_frame << ", from = " <<
        // node_from << ", recv = " << node_recv;

        for (int j = 0; j < 100; j++) {
            // allMessage[currentTimestamp].points[node_from-1].y
            // allMessage[currentTimestamp].points[node_recv-1].y
            // 这是一个点。
            double cur_x =
                allMessage[currentTimestamp].points[node_from - 1].x +
                (allMessage[currentTimestamp].points[node_recv - 1].x -
                 allMessage[currentTimestamp].points[node_from - 1].x) *
                    (j) * 0.01;

            double cur_y =
                allMessage[currentTimestamp].points[node_from - 1].y +
                (allMessage[currentTimestamp].points[node_recv - 1].y -
                 allMessage[currentTimestamp].points[node_from - 1].y) *
                    (j) * 0.01;

            TimePoint &timePoints_xxxx = allMessage[first_frame + j];
            QVector<Point> &temp_points_xxxx = timePoints_xxxx.points;

            // temp_points_xxxx.append( {cur_x, cur_y, 0, -1, 50, 50});
            temp_points_xxxx.append({node_from, node_recv, 0, 996, 50, 50});
        }
    }

    // 下面开始写

    // 开始北伐 ！！！ 感觉逻辑最难盘的一部分。
    QJsonObject aodvMessage = aodvObj.value("Message").toObject();

    // json 文件里的 时间单位是 ns, 就是 10^-9 s. 换算成帧的话需要 * 0.0000001
    // 直接把后面 7+2位 截除。
    QStringList packet_id = aodvMessage.keys();

    for (int i = 0; i < packet_id.size(); i++) {
        QJsonObject cur_message = aodvMessage[packet_id.at(i)].toObject();

        QStringList packet_trace = cur_message.keys();

        QString frame = packet_trace.at(0);
        int pre_node = cur_message[frame].toInt();
        frame.chop(7 + 2);
        int pre_frame = frame.toInt();

        // 如果有问题的话就需要从前往后
        for (int j = 1; j < packet_trace.size(); j++) {
            QString frame = packet_trace.at(j);
            int cur_node = cur_message[frame].toInt();
            frame.chop(7 + 2);
            // qDebug() << frame;
            int cur_frame = frame.toInt();
            // qDebug() << "pre_frame = " << pre_frame << ", cur_frame = " <<
            // cur_frame;

            // 从前向后输出
            // pre_node->cur_node
            // pre_frame->cur_frame
            if (pre_frame > cur_frame) {
                qDebug() << "Error!";
            }
            // qDebug() << pre_frame;

            // 多画半秒方便观看
            for (int temp_frame = pre_frame; temp_frame <= cur_frame + 50;
                 temp_frame++) {
                double cur_x;
                double cur_y;
                if (pre_frame == cur_frame) {
                    cur_x = node_pos[pre_node][1];
                    cur_y = node_pos[pre_node][2];
                } else {
                    cur_x =
                        allMessage[currentTimestamp].points[pre_node - 1].x +
                        1.0 * (temp_frame - pre_frame) /
                            (cur_frame - pre_frame + 50) *
                            (allMessage[currentTimestamp]
                                 .points[cur_node - 1]
                                 .x -
                             allMessage[currentTimestamp]
                                 .points[pre_node - 1]
                                 .x);

                    cur_y =
                        allMessage[currentTimestamp].points[pre_node - 1].y +
                        1.0 * (temp_frame - pre_frame) /
                            (cur_frame - pre_frame + 50) *
                            (allMessage[currentTimestamp]
                                 .points[cur_node - 1]
                                 .y -
                             allMessage[currentTimestamp]
                                 .points[pre_node - 1]
                                 .y);
                }

                TimePoint &timePoints_xxxx = allMessage[temp_frame];
                QVector<Point> &temp_points_xxxx = timePoints_xxxx.points;
                // temp_points_xxxx.append( {cur_x, cur_y, 0, -1, 50, 50});
                temp_points_xxxx.append({pre_node, cur_node, 0, 996, 50, 50});

                // qDebug() << "cur_frame = " << cur_frame << ", cur_x = " <<
                // cur_x << ", cur_y = " << cur_y;
            }
            pre_node = cur_node;
            pre_frame = cur_frame;
        }

        // int node_from = cur_message["message_from"].toDouble();
        // int node_recv = cur_message["message_receive"].toDouble();
        // int mege_size = cur_message["message_size"].toDouble();
        // int first_frame = cur_message["time"].toDouble() * 100;

        // qDebug() << "first_frame = " << first_frame << ", from = " <<
        // node_from << ", recv = " << node_recv;

        // for (int j = 0; j < 100; j ++) {
        //     // 这是一个点。
        //     double cur_x = node_pos[node_from][1] + (node_pos[node_recv][1] -
        //     node_pos[node_from][1])*(j) * 0.01; double cur_y =
        //     node_pos[node_from][2] + (node_pos[node_recv][2] -
        //     node_pos[node_from][2])*(j) * 0.01;

        //     TimePoint &timePoints_xxxx = allMessage[first_frame+j];
        //     QVector<Point>& temp_points_xxxx = timePoints_xxxx.points;
        //     temp_points_xxxx.append( {cur_x, cur_y, -1});
        // }
    }

    // 下面都是之前测试用的代码。
    //     for (int i = 0; i < 2000; i++){
    //         tp.points.append({(2 * i) % 500 + 199, 200, -1});
    //         tp.points.append({300, (2 * i) % 500 + 199, -1});
    //         tp.points.append({(2 * i) % 500 + 199, (2 * i) % 500 + 199, -1});
    // //            tp.points.append({100*i, 200});
    // //            tp.points.append({200*i, 200});
    // //            tp.points.append({300*i, 200});
    //         allMessage.append(tp);
    //         tp.points.clear();
    //     }

    // for (int j = 0; j < 100; j++)
    // for (int i = 0; i < 1000; i++) {
    //     TimePoint &timePoints = allMessage[i];
    //     QVector<Point>& temp_points = timePoints.points;
    //     temp_points.append( {100+j,100+2*j,-1});
    // }
}

Widget::Widget(QWidget *parent) : QWidget(parent) {



    x_coefficient = 1.0*cur_width/default_width;
    y_coefficient = 1.0*cur_height/default_height;
    qDebug() << "create Widget!" << " x_coefficient = " <<x_coefficient << ", y_coefficient = " << y_coefficient;
    // this->resize(2000, 2000);

    btn1 = new QPushButton("Start", this);
    btn2 = new QPushButton("Stop", this);
    btn3 = new QPushButton("Cut Show", this);
    lineEdit = new QLineEdit(this);

    label1 = new QLabel(this);
    label2 = new QLabel(this);
    // output = new QTextEdit(this);
    label1->setGeometry(360, 10, 50, 30);
    label1->setText("Frame:");
    label2->setGeometry(420, 10, 40, 30);

    btn1->setGeometry(10, 10, 80, 30);
    btn2->setGeometry(100, 10, 80, 30);
    lineEdit->setGeometry(190, 10, 80, 30);
    btn3->setGeometry(280, 10, 80, 30);

    label2->setText("0");

    // output->setGeometry(100, 770, 500, 75);

    canvas_origin[0][0] = 100;
    canvas_origin[0][1] = 250;
    canvas_origin[1][0] = 100;
    canvas_origin[1][1] = 500;
    canvas_origin[2][0] = 100;
    canvas_origin[2][1] = 750;
    canvas_height = 223;
    canvas_width = 1000;
    // 设置宽度。
    this->drawScene();



    pix_img1 = QPixmap("/home/eynnzerr/open/JetBrainsWorkSpace/CLion/ns338/"
                       "Painter10.13/source/auv.png");
    pix_img1 = pix_img1.scaled(50, 50);
    pix_img2 = QPixmap("/home/eynnzerr/open/JetBrainsWorkSpace/CLion/ns338/"
                       "Painter10.13/source/uav.png");
    pix_img2 = pix_img2.scaled(50, 50);
    pix_img3 = QPixmap("/home/eynnzerr/open/JetBrainsWorkSpace/CLion/ns338/"
                       "Painter10.13/source/usv.png");
    pix_img3 = pix_img3.scaled(50, 50);
    pix_img4 = QPixmap("/home/eynnzerr/open/JetBrainsWorkSpace/CLion/ns338/"
                       "Painter10.13/source/test.jpg");
    pix_img4 = pix_img4.scaled(50, 50);

    double node_pos[100][5];
    for (int i = 0; i < 100; i++) {
        for (int j = 0; j < 5; j++) {
            node_pos[i][j] = 0;
        }
    }

    char *filename = "/home/eynnzerr/open/JetBrainsWorkSpace/CLion/"
                     "ns-allinone-3.38/ns-3.38/2d-plot.json";
    // this->parse_json(filename);

    // Create a timer to refresh the points every 20ms
    connect(btn1, &QPushButton::clicked, [=]() {
        timer = new QTimer(this);

        //    qDebug() << allMessage.size();
        connect(timer, &QTimer::timeout, [=]() { updatePosition(); });
        timer->start(10);
        connect(btn2, &QPushButton::clicked, timer, &QTimer::stop);
    });
    connect(btn3, &QPushButton::clicked, [=]() { showPosition(); });

}

Widget::~Widget() {}

void Widget::updatePosition() {
    if (currentTimestamp < allMessage.size()) {
        currentTimestamp++;
    } else {
        currentTimestamp = 0;
    }

    QString frame = QString::number(currentTimestamp, 10);
    label2->setText(frame);
    update();
}

void Widget::setCoefficient(int cur_w, int cur_h) {
    cur_width = cur_w; cur_height = cur_h;
    x_coefficient = 1.0*cur_width/default_width;
    y_coefficient = 1.0*cur_height/default_height;
    this->clearScene();
    this->drawScene();
}

void Widget::clearScene() {
    parallelogram_sky.clear();
}

void Widget::drawScene () {
    parallelogram_sky.moveTo(200*x_coefficient, 50*y_coefficient);
    parallelogram_sky.lineTo(1200*x_coefficient, 50*y_coefficient);
    parallelogram_sky.lineTo(1100*x_coefficient, 250*y_coefficient);
    parallelogram_sky.lineTo(100*x_coefficient, 250*y_coefficient);
    parallelogram_sky.lineTo(200*x_coefficient, 50*y_coefficient);

    // parallelogram_land.moveTo(200, 300);
    // parallelogram_land.lineTo(1200, 300);
    // parallelogram_land.lineTo(1100, 500);
    // parallelogram_land.lineTo(100, 500);
    // parallelogram_land.lineTo(200, 300);

    // parallelogram_underwater.moveTo(200, 550);
    // parallelogram_underwater.lineTo(1200, 550);
    // parallelogram_underwater.lineTo(1100, 750);
    // parallelogram_underwater.lineTo(100, 750);
    // parallelogram_underwater.lineTo(200, 550);
}

void Widget::DrawLineWithArrow(QPainter &painter, QPen pen, QPoint start,
                               QPoint end) {
    painter.setRenderHint(QPainter::Antialiasing, true);

    qreal arrowSize = 20;
    painter.setPen(pen);
    painter.setBrush(pen.color());

    QLineF line(end, start);

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
    painter.drawLine(line);
    painter.drawPolygon(arrowHead);
}

void Widget::showPosition() {
    if (lineEdit->text() != NULL) {
        currentTimestamp = lineEdit->text().toInt();
    }
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

    if (!allMessage.empty()) {
        const TimePoint &timePoints = allMessage[currentTimestamp];
        const QVector<Point> &points = timePoints.points;

        for (const Point &point : points) {
            double x = point.x;
            double y = point.y;
            if (point.node_type == 1) {
                painter.drawPixmap(
                    x - point.node_width * 0.5, y - point.node_height * 0.5,
                    point.node_width, point.node_height, pix_img1);
            } else if (point.node_type == 2) {
                painter.drawPixmap(
                    x - point.node_width * 0.5, y - point.node_height * 0.5,
                    point.node_width, point.node_height, pix_img2);
            } else if (point.node_type == 3) {
                painter.drawPixmap(
                    x - point.node_width * 0.5, y - point.node_height * 0.5,
                    point.node_width, point.node_height, pix_img3);
            } else if (point.node_type == 996) {
                int first = (int)x;
                int second = (int)y;
                if (first == second) {
                    continue;
                }
                int x1 = points[first - 1].x;
                int y1 = points[first - 1].y;
                int x2 = points[second - 1].x;
                int y2 = points[second - 1].y;
                // pen2.setColor(QColor(line.color_r, line.color_g,
                // line.color_b));
                DrawLineWithArrow(painter, pen2, QPoint(x1, y1),
                                  QPoint(x2, y2));
            } else {
                qDebug() << "points.size() = " << points.size();
                // painter.drawPixmap(x-point.node_width*0.5,
                // y-point.node_height*0.5, point.node_width, point.node_height,
                // pix_img4);
            }
        }
    }

    // int cur_second = currentTimestamp / 100;
    // if (cur_second > aqua_line_message.size()) {

    // } else {
    //     lines_aodv = aodv_line_message[cur_second];
    //     lines_aqua = aqua_line_message[cur_second];
    // }
    // const TimePoint &timePoints = allMessage[currentTimestamp];
    // const QVector<Point>& points = timePoints.points;

    // for (const Line& line : lines_aqua.lines) {
    //     int first = line.begin_id;
    //     int second = line.end_id;
    //     int x1 = points[first].x;
    //     int y1 = points[first].y;
    //     int x2 = points[second].x;
    //     int y2 = points[second].y;
    //     pen2.setColor(QColor(line.color_r, line.color_g, line.color_b));
    //     DrawLineWithArrow(painter, pen2, QPoint(x1, y1), QPoint(x2, y2));
    // }

    // for (const Line& line : lines_aodv.lines) {
    //     int first = line.begin_id;
    //     int second = line.end_id;
    //     int x1 = points[first].x;
    //     int y1 = points[first].y;
    //     int x2 = points[second].x;
    //     int y2 = points[second].y;
    //     pen2.setColor(QColor(line.color_r, line.color_g, line.color_b));
    //     DrawLineWithArrow(painter, pen2, QPoint(x1, y1), QPoint(x2, y2));
    // }
}
