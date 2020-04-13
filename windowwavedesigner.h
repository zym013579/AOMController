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
     * @brief update_myGraph 更新本窗口的图像
     */
    void update_myGraph();

    /**
     * @brief update_pointText 根据鼠标选点或拖动操作的结果，来更新lineEdit中显示的数据
     */
    void update_pointText();

    /**
     * @brief check_pointText 在鼠标焦点离开lineEdit时，检查其中数据的准确性
     */
    void check_pointText();

    /**
     * @brief update_pointNumber 根据输入的点序号，实时选择点（若存在，此时不提示错误）
     */
    void update_pointNumber();

    /**
     * @brief update_pointData 根据输入的点数据，实时更新图像（若存在，此时不提示错误）
     */
    void update_pointData();

    /**
     * @brief save_step 保存当前图形至历史记录
     */
    void save_step();

    /**
     * @brief drop_step 放弃对当前图形的更改
     */
    void drop_step();

    /**
     * @brief undo_step 撤销一步
     */
    void undo_step();

    /**
     * @brief redo_step 还原一步
     */
    void redo_step();

    /**
     * @brief fresh_undo_redo_button 改变“撤销”“恢复”按钮的可用状态
     */
    void fresh_undo_redo_button();

    /**
     * @brief if_pointClicked 检测当前是否点击了某点
     * @param event 鼠标点击事件QMouseEvent
     * @return 点击的点序号，若未点击返回-1
     */
    int if_pointClicked(QMouseEvent *event);

    /**
     * @brief witch_pointClicked 是否有点被选中
     * @return 被选中的点序号，若未选中返回-1
     */
    int witch_pointClicked();

    /**
     * @brief choose_point 选中某个点
     * @param i 选中点的序号，其中-1代表取消选点
     */
    void choose_point(int i);

    void on_pushButtonNew_clicked();

    void widgetMousePress(QMouseEvent *event);

    void widgetMouseRelease(QMouseEvent *event);

    void widgetMouseMove(QMouseEvent *event);

    void on_pushButtonSave_clicked();

    void on_pushButtonDelete_clicked();

private:
    Ui::WindowWaveDesigner *ui;

    WaveData *edit;

    bool mouseHasMoved;

    /**
     * @param c_point 点击已选中的点时改变值
     */
    int c_point, point_circleSize;

    double disX, minDisX;

    void closeEvent(QCloseEvent *event);
signals:
    void send_waveData(WaveData *data);
};

void init_waveGraph(QCustomPlot *target);

void update_waveGraph(QCustomPlot *target, QList<double> x, QList<double> y);

/**
 * @brief check_data 检测输入数据是否超出范围
 * @param input 输入值
 * @param min 范围最小值
 * @param max 范围最大值
 * @return 若输入值在区间内则返回输入值，否则返回临界值
 */
double check_data(double input, double min, double max);

#endif // WINDOWWAVEDESIGNER_H
