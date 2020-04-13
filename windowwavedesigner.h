#ifndef WINDOWWAVEDESIGNER_H
#define WINDOWWAVEDESIGNER_H

#define MAX(a,b)  (((a)>(b))?(a):(b))
#define MIN(a,b)  (((a)<(b))?(a):(b))

#define X_VAL (ui->widgetWave->xAxis->pixelToCoord(event->pos().x()))
#define Y_VAL (ui->widgetWave->yAxis->pixelToCoord(event->pos().y()))

#define X_RANGE ((ui->widgetWave->width()-34-15)/30.0*5/ui->widgetWave->xAxis->range().size()*11/point_circleSize)
#define Y_RANGE ((ui->widgetWave->height()-14-24)/6.2/ui->widgetWave->yAxis->range().size()*11/point_circleSize)

#include <QMainWindow>
#include <QCloseEvent>
#include <QMessageBox>
//#include <QtDebug>
#include <QtMath>

#include "qcustomplot.h"
#include "wavedata.h"

namespace Ui {
class WindowWaveDesigner;
}

class WindowWaveDesigner : public QMainWindow
{
    Q_OBJECT

public slots:
    void recieve_waveData(WaveData *data);

public:
    explicit WindowWaveDesigner(QWidget *parent = nullptr);
    ~WindowWaveDesigner();

private slots:
    /**
     * @brief 更新本窗口的图像
     */
    void update_myGraph();

    /**
     * @brief 根据鼠标选点或拖动操作的结果，来更新lineEdit中显示的数据
     */
    void update_pointText();

    /**
     * @brief 在鼠标焦点离开lineEdit时，检查其中数据的准确性
     */
    void check_pointText();

    /**
     * @brief 根据输入的点序号，实时选择点（若存在，此时不提示错误）
     */
    //void update_pointNumber();

    /**
     * @brief 根据输入的点数据，实时更新图像（若存在，此时不提示错误）
     */
    //void update_pointData();

    /**
     * @brief 保存当前图形至历史记录
     */
    void save_step();

    /**
     * @brief 放弃对当前图形的更改
     */
    void drop_step();

    /**
     * @brief 撤销一步
     */
    void undo_step();

    /**
     * @brief 还原一步
     */
    void redo_step();

    /**
     * @brief 改变“撤销”“恢复”按钮的可用状态
     */
    void fresh_undo_redo_button();

    /**
     * @brief 检测当前是否点击了某点
     * @param event 鼠标点击事件QMouseEvent
     * @return 点击的点序号，若未点击返回-1
     */
    int if_pointClicked(QMouseEvent *event);

    /**
     * @brief 是否有点被选中
     * @return 被选中的点序号，若未选中返回-1
     */
    int witch_pointClicked();

    /**
     * @brief 选中某个点
     * @param i 选中点的序号，其中-1代表取消选点
     */
    void choose_point(int i);

    /**
     * @brief 窗口关闭事件
     * @param event 事件
     */
    void closeEvent(QCloseEvent *event);

    void on_pushButtonNew_clicked();

    /**
     * @brief 鼠标按下事件
     * @param event 事件
     */
    void widgetMousePress(QMouseEvent *event);

    /**
     * @brief 鼠标释放事件
     * @param event 事件
     */
    void widgetMouseRelease(QMouseEvent *event);

    /**
     * @brief 鼠标移动事件
     * @param event 事件
     */
    void widgetMouseMove(QMouseEvent *event);

    void on_pushButtonSave_clicked();

    void on_pushButtonDelete_clicked();

    void on_lineEditPointNumber_textEdited(const QString &arg1);

    void on_lineEditPointTime_textEdited(const QString &arg1);

    void on_lineEditPointVoltage_textEdited(const QString &arg1);

private:
    Ui::WindowWaveDesigner *ui;

    WaveData *edit;

    bool mouseHasMoved;

    /**
     * @param c_point 点击已选中的点时改变值
     */
    int c_point, point_circleSize;

    double disX, minDisX;

signals:
    void send_waveData(WaveData *data);
};

void init_waveGraph(QCustomPlot *target);

void update_waveGraph(QCustomPlot *target, QList<double> x, QList<double> y);

/**
 * @brief 检测输入数据是否超出范围
 * @param input 输入值
 * @param min 范围最小值
 * @param max 范围最大值
 * @return 若输入值在区间内则返回输入值，否则返回临界值
 */
double check_data(double input, double min, double max);

#endif // WINDOWWAVEDESIGNER_H
