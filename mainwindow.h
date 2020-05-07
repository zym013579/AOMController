#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>

#include "qcustomplot.h"
#include "wavedata.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    /*!
     * \brief 更新本窗口的正在调制图像
     */
    void updateModuGraph();

    /*!
     * \brief 更新本窗口的正在编辑图像
     */
    void updateEditGraph();

    /*!
     * \brief 接收波形信息
     * \param data
     *
     * 与其他窗口的Signal使用Connect连接，\a *data 为传入数据，传入数据赋值给edit
     */
    void recieveWaveDataFromEditor(WaveData *data);

    /*!
     * \brief 打开端口
     * \return 打开成功或失败
     */
    bool openPort();

    /*!
     * \brief 关闭端口
     */
    void closePort();

    /*!
     * \brief 设置默认串口参数
     */
    void setPortConfig();

    /*!
     * \brief 扫描串口设备，将串口名更新至组合框中
     */
    void searchDevice();

    /*!
     * \brief 接收下位机信息，并判断是否为成功信号，可指定超时时间\a timeout
     * \return 是否成功
     */
    bool recieveDeviceInfo(int timeout = 3000);

    /*!
     * \brief 向下位机发送字符串\a *data ，指定发送字符长度\a length ，-1为不限制，可指定超时时间\a timeout
     * \param data
     * \param length
     * \param timeout
     * \return
     */
    bool sendToDevice(QByteArray *data, int length = -1, int timeout = 3000);

    /*!
     * \brief 向下位机发送指令\a command ，目前可用"start"，"pause"，"test"
     * \param command
     * \return
     */
    bool sendCommandToDevice(QString command);

    void on_pushButtonEditWave_clicked();

    void on_actionAbout_triggered();

    /*!
     * \brief 开始调制按钮
     */
    void on_pushButtonStart_clicked();

    /*!
     * \brief 发送波形信息按钮
     */
    void on_pushButtonSend_clicked();

    /*!
     * \brief 连接下位机按钮
     */
    void on_pushButtonConnect_clicked();

    /*!
     * \brief 设备改变事件，插入或移出设备时触发
     * \param eventType
     * \param message
     * \param result
     * \return
     */
    bool nativeEvent(const QByteArray &eventType, void *message, long *result);

    /*!
     * \brief 上一个波形按钮
     */
    void on_actionPreWave_triggered();

    /*!
     * \brief 刷新上一个/下一个波形按钮的可用状态
     */
    void freshUndoRedoButton();

    /*!
     * \brief 下一个波形按钮
     */
    void on_actionNextWave_triggered();

private:
    Ui::MainWindow *ui;

    WaveData *modu, *edit;

    QStringList m_serialPortName;

    QSerialPort *m_serialPort;

signals:
    void sendSettings(WaveData *data);
};

/*!
 * \brief 初始化图形widget
 * \param target
 */
void initWaveGraph(QCustomPlot *target);

/*!
 * \brief 更新目标widget图形（底层）
 * \param target 目标widget
 * \param x
 * \param y
 */
void updateWaveGraph(QCustomPlot *target, QList<double> x, QList<double> y, double maxVol);


/*!
 * \brief 将数字转为字符串，QString自带函数有一些小问题
 * \param num
 * \return
 */
QString numberToStr(int num);

QString numberToStr(double num);

#endif // MAINWINDOW_H
