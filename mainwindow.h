#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "windowwavedesigner.h"
#include "dialogabout.h"

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
     * @brief update_myModuGraph 更新本窗口的正在调制图像
     */
    void update_myModuGraph();

    /**
     * @brief update_myEditGraph 更新本窗口的正在编辑图像
     */
    void update_myEditGraph();

    /**
     * @brief recieve_waveData 接收波形信息
     * @param data 传入的波形信息
     */
    void recieve_waveData(WaveData *data);

    void on_pushButtonEditWave_clicked();

    void on_actionAbout_triggered();

    void on_pushButtonStart_clicked();

    void on_pushButtonSend_clicked();

    void on_pushButtonConnect_clicked();

    void connect_success();
private:
    Ui::MainWindow *ui;

    WindowWaveDesigner *wDesigner;

    DialogAbout *wAbout;

    WaveData *modu, *edit;
};

/**
 * @brief init_waveGraph 初始化图形widget
 * @param target 目标widget
 */
void init_waveGraph(QCustomPlot *target);

/**
 * @brief update_waveGraph 更新目标widget图形（底层）
 * @param target 目标widget
 * @param x x轴数据
 * @param y y轴数据
 */
void update_waveGraph(QCustomPlot *target, QList<double> x, QList<double> y);

double check_data(double input, double min, double max);

#endif // MAINWINDOW_H
