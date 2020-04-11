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
    void update_myModuGraph();

    void update_myEditGraph();

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

void init_waveGraph(QCustomPlot *target);
void update_waveGraph(QCustomPlot *target, QVector<double> x, QVector<double> y);
double check_data(double input, double min, double max);

#endif // MAINWINDOW_H
