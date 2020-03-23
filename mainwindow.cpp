#include "mainwindow.h"
#include "ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    init_waveGraph(ui->widgetModulatingWave);
    init_waveGraph(ui->widgetEditingWave);
}

MainWindow::~MainWindow()
{
    delete ui;
}

/*
初始化图形界面，包括颜色等设置
*/
void init_waveGraph(QCustomPlot *target)
{
    target->clearGraphs();
    target->addGraph();
    target->yAxis->setRange(0, 1);
    target->replot();
    return;
}

/*
用x,y来更新图形
*/
void update_waveGraph(QCustomPlot *target, QVector<double> x, QVector<double> y)
{
    target->graph(0)->setData(x, y);
    //target->xAxis->setLabel("x");
    //target->yAxis->setLabel("y");
    target->xAxis->setRange(0, 6.28);
    //target->yAxis->setRange(-1, 1);
    target->replot();
    return;
}

void MainWindow::on_pushButtonEditWave_clicked()
{
    WindowWaveDesigner *wDesigner = new WindowWaveDesigner;
    wDesigner->show();
    return;
}

void MainWindow::on_actionAbout_triggered()
{
    DialogAbout *wAbout = new DialogAbout;
    wAbout->exec();
    return;
}

/*临时用作测试*/
void MainWindow::on_pushButtonStart_clicked()
{
    QVector<double> x(100000), y(100000);
    for (int i = 0; i < 6280; i++) {
        x[i] = i / 1000.0;
        y[i] = sin(x[i]);
    }
    //update_waveGraph(ui->widgetModulatingWave, x, y);
    ui->widgetModulatingWave->setGeometry(5, 10, 800, 200);
}

void MainWindow::on_pushButtonSend_clicked()
{
    //QVector<double> x(100000), y(100000);
    //update_waveGraph(ui->widgetModulatingWave, x, y);
}
