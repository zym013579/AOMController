#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>

#include "windowwavedesigner.h"
#include "dialogabout.h"
#include "signal.h"
#include <dbt.h>


//#include "qcustomplot.h"
//#include "wavedata.h"

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
    void update_myModuGraph();

    /**
     * @brief 更新本窗口的正在编辑图像
     */
    void update_myEditGraph();

    /**
     * @brief 接收波形信息
     * @param data 传入的波形信息
     */
    void recieve_waveData(WaveData *data);

    bool openPort();

    void closePort();

    /**
     * @brief 尝试连接下位机
     * @return 0成功
     */
    bool try_toConnect();

    /**
     * @brief 设置串口参数
     */
    void setPortConfig();

    /**
     * @brief 连接下位机成功
     */
    void connected();

    /**
     * @brief 下位机连接断开、丢失
     */
    void disconnected();

    /**
     * @brief 开始调制
     */
    void modulating();

    /**
     * @brief 调制状态中断
     */
    void modulate_interrupted();

    /**
     * @brief 扫描串口设备
     */
    void search_device();

    /**
     * @brief 接收下位机信息
     */
    void recieveInfo();

    void on_pushButtonEditWave_clicked();

    void on_actionAbout_triggered();

    void on_pushButtonStart_clicked();

    void on_pushButtonSend_clicked();

    void on_pushButtonConnect_clicked();

    bool nativeEvent(const QByteArray &eventType, void *message, long *result);
private:
    Ui::MainWindow *ui;

    WindowWaveDesigner *wDesigner;

    DialogAbout *wAbout;

    WaveData *modu, *edit;

    QStringList m_serialPortName;

    QSerialPort *m_serialPort;

    bool m_connectStatus;
};

/**
 * @brief 初始化图形widget
 * @param target 目标widget
 */
void init_waveGraph(QCustomPlot *target);

/**
 * @brief 更新目标widget图形（底层）
 * @param target 目标widget
 * @param x x轴数据
 * @param y y轴数据
 */
void update_waveGraph(QCustomPlot *target, QList<double> x, QList<double> y);

//double check_data(double input, double min, double max);

#endif // MAINWINDOW_H
