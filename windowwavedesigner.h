#ifndef WINDOWWAVEDESIGNER_H
#define WINDOWWAVEDESIGNER_H

#include <QMainWindow>
#include <QCloseEvent>

#include "qcustomplot.h"
#include "wavedata.h"


namespace Ui {
class WindowWaveDesigner;
}

class WindowWaveDesigner : public QMainWindow
{
    Q_OBJECT

public:
    explicit WindowWaveDesigner(QWidget *parent = nullptr);
    ~WindowWaveDesigner();

private slots:
    void recieveSettings(WaveData *data);
    /*!
     * \brief 更新本窗口的图像
     */
    void updateGraph();

    /*!
     * \brief 根据鼠标选点或拖动操作的结果，来更新lineEdit中显示的数据
     */
    void updateLineEditText();

    /*!
     * \brief 保存当前图形至历史记录
     */
    void saveStep();

    /*!
     * \brief 放弃对当前图形的更改
     */
    void dropStep();

    /*!
     * \brief 撤销一步
     */
    void undoStep();

    /*!
     * \brief 还原一步
     */
    void redoStep();

    /*!
     * \brief 改变“撤销”“恢复”按钮的可用状态
     */
    void freshUndoRedoButton();

    /*!
     * \brief 检测当前是否点击了某点
     * \param event 鼠标点击事件QMouseEvent
     * \return 点击的点序号，若未点击返回-1
     */
    int witchPointclicking(QMouseEvent *event);

    /*!
     * \brief 是否有点被选中
     * \return 被选中的点序号，若未选中返回-1
     */
    int witchPointclicked();

    /*!
     * \brief 选中某些点，内已调用updateGraph、updateLineEditText函数
     * \param i 选中点的序号，其中-1代表取消选点
     * \param j 默认-1为选单点，否则为选i至j多个点
     */
    void choosePoint(int i, int j = -1);

    /*!
     * \brief 窗口关闭事件
     * \param event
     */
    void closeEvent(QCloseEvent *event);

    void on_pushButtonNew_clicked();

    /*!
     * \brief 鼠标按下事件
     * \param event
     */
    void on_widgetWave_mousePress(QMouseEvent *event);

    /*!
     * \brief 鼠标释放事件
     * \param event
     */
    void on_widgetWave_mouseRelease(QMouseEvent *event);

    /*!
     * \brief 鼠标移动事件
     * \param event
     */
    void on_widgetWave_mouseMove(QMouseEvent *event);

    /*!
     * \brief 应用波形按钮
     */
    void on_pushButtonSave_clicked();

    void on_pushButtonDelete_clicked();

    void on_lineEditPointNumber_textEdited(const QString &arg1);

    void on_lineEditPointTime_textEdited(const QString &arg1);

    void on_lineEditPointVoltage_textEdited(const QString &arg1);

    void on_lineEditPointNumber_editingFinished();

    void on_lineEditPointTime_editingFinished();

    void on_lineEditPointVoltage_editingFinished();

    void on_pushButtonInsert_clicked();

    /*!
     * \brief 应用设置按钮，非应用波形
     */
    void on_pushButtonApply_clicked();

    void on_actionOpenFile_triggered();

    void on_actionClear_triggered();

    void on_actionSaveAsFile_triggered();

private:
    Ui::WindowWaveDesigner *ui;

    WaveData *edit;

    bool mouseHasMoved, saved;

    /*!
     * \param c_point 点击已选中的点时改变值
     */
    int c_point;

signals:
    void sendWaveData(WaveData *data);
};

void initWaveGraph(QCustomPlot *target);

void updateWaveGraph(QCustomPlot *target, QList<double> x, QList<double> y, double maxVol);

/*!
 * \brief 检测输入数据是否超出范围
 * \param input 输入值
 * \param min 范围最小值
 * \param max 范围最大值
 * \return 若输入值在区间内则返回输入值，否则返回临界值
 */
double checkData(double input, double min, double max);

QString numberToStr(int num);

QString numberToStr(double num);

#endif // WINDOWWAVEDESIGNER_H
