#ifndef TOPOWIDGETH
#define TOPOWIDGETH

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QPushButton>
#include <QTimer>
#include <QVector>
#include <QLineEdit>
#include <QJsonObject>

#include <QLabel>

#include <QLineEdit>
#include <QPainterPath>
#include <QTextEdit>

class TopoWidget : public QWidget
{
    Q_OBJECT

struct Point{
    double x;
    double y;
    double z;
    // 定义一个
    int node_type;
    int node_height;
    int node_width;
};

struct Line{
    int begin_id;
    int   end_id;
    int color_r;
    int color_g;
    int color_b;
};
// 不理解这里为什么是一个结构体
// 感觉没有必要。 
struct TimePoint{
    QVector<Point> points;
};

struct TimeLine{
    QVector<Line> lines;
};

private:
    int currentTimestamp = 0;
    char *configPath;

    QLineEdit* lineEdit;
    TimePoint tp;
    QVector<TimePoint> allMessage;

    QVector<TimePoint> node_pos_Message;

    TimeLine tl;
    QVector<TimeLine> aodv_line_message;
    QVector<TimeLine> aqua_line_message;
    QVector<TimeLine> sound_line_message;

    TimeLine lines_aodv;
    TimeLine lines_aqua;
    TimeLine lines_light;

    QPainterPath parallelogram_sky;
    QPainterPath parallelogram_land;
    QPainterPath parallelogram_underwater;
    
    QPixmap pix_img1;
    QPixmap pix_img2;
    QPixmap pix_img3;
    QPixmap pix_img4;

    int canvas_height = 200;
    int canvas_width  = 1000;
    // 空 地 海  
    // x , y
    int canvas_origin[3][2];

    // node_pos[i] , 分别为： 节点种类， 节点x, 节点y, 节点z, 该节点是否真的存在
    double node_pos[100][5];

    int default_width  = 1200;
    int default_height = 950;
    int cur_width  = 385 - 100;
    int cur_height = 269 - 100;

    double x_coefficient;
    double y_coefficient;

public:
    TopoWidget(QWidget *parent = nullptr);
    TopoWidget(char *configPath,QWidget *parent = nullptr);


    ~TopoWidget();
    void init_widget ();
    void parse_json(const char* filename);

    void paintEvent(QPaintEvent *event) override;
    void updatePosition();
    void showPosition();
    void DrawLineWithArrow(QPainter& painter, QPen pen, QPoint start, QPoint end, int with_arrow);

    void clearScene();
    void drawScene ();
    void setCoefficient(int cur_w, int cur_h);
    int getTotalTime();
};
#endif // TOPOWIDGETH
