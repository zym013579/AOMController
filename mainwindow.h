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
    /**
     * @brief 更新本窗口的正在调制图像
     */
    void updateModuGraph();

    /**
     * @brief 更新本窗口的正在编辑图像
     */
    void updateEditGraph();

    /**
     * @brief 接收波形信息
     * @param data 传入的波形信息
     */
    void recieveWaveDataFromEditor(WaveData *data);

    /**
     * @brief 打开端口
     * @return 打开成功或失败
     */
    bool openPort();

    /**
     * @brief 关闭端口
     */
    void closePort();

    /**
     * @brief 设置串口参数
     */
    void setPortConfig();

    /**
     * @brief 扫描串口设备
     */
    void searchDevice();

    /**
     * @brief 接收下位机信息
     * @return 是否成功
     */
    bool recieveDeviceInfo(int timeout = 3000);

    bool sendToDevice(QByteArray *data, int length = -1, int timeout = 3000);

    bool sendCommandToDevice(QString command);

    void on_pushButtonEditWave_clicked();

    void on_actionAbout_triggered();

    void on_pushButtonStart_clicked();

    void on_pushButtonSend_clicked();

    void on_pushButtonConnect_clicked();

    /**
     * @brief 设备改变事件
     * @param eventType
     * @param message
     * @param result
     * @return
     */
    bool nativeEvent(const QByteArray &eventType, void *message, long *result);

private:
    Ui::MainWindow *ui;

    WaveData *modu, *edit;

    QStringList m_serialPortName;

    QSerialPort *m_serialPort;

signals:
    void sendSettings(WaveData *data);
};

/**
 * @brief 初始化图形widget
 * @param target 目标widget
 */
void initWaveGraph(QCustomPlot *target);

/**
 * @brief 更新目标widget图形（底层）
 * @param target 目标widget
 * @param x x轴数据
 * @param y y轴数据
 */
void updateWaveGraph(QCustomPlot *target, QList<double> x, QList<double> y, double maxVol);

QString numberToStr(int num);

QString numberToStr(double num);

#endif // MAINWINDOW_H
