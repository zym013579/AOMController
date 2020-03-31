#include "mainwindow.h"
#include "ui_mainwindow.h"

//QVector<double> WaveData::editX, WaveData::editY, WaveData::moduX, WaveData::moduY, WaveData::tempX, WaveData::tempY;
//double WaveData::deltaX, WaveData::voltage;
//WaveData WaveData;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    modu(new WaveData),
    edit(new WaveData)
{
    ui->setupUi(this);
    init_waveGraph(ui->widgetModulatingWave);
    init_waveGraph(ui->widgetEditingWave);
    //WaveData::tempX.append(0);
    //WaveData::tempY.append(0);
    //waveModuX[1]->append(2);
}

MainWindow::~MainWindow()
{
    modu->clear();
    edit->clear();
    delete modu;
    delete edit;
    delete ui;
}

void MainWindow::connect_success()
{
    ui->pushButtonStart->setEnabled(true);
    ui->pushButtonSend->setEnabled(true);
    ui->lineEditConnectStatus->setText("已连接，未调制");
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
}

/*
用x,y来更新图形
*/
void update_waveGraph(QCustomPlot *target, QVector<double> x, QVector<double> y)
{
    target->graph(0)->setData(x, y);
    //target->xAxis->setLabel("x");
    //target->yAxis->setLabel("y");
    //x.remove(0,5);
    //target->xAxis->setRange(x[0], x[x.size()-1]+0.5);
    //target->yAxis->setRange(-1, 1);
    target->replot();
}

void reset_waveGraphAxis(QCustomPlot *target)
{
    target->rescaleAxes();
}

void MainWindow::update_myModuGraph()
{
    update_waveGraph(ui->widgetModulatingWave, modu->x(), modu->y());
}

void MainWindow::update_myEditGraph()
{
    update_waveGraph(ui->widgetEditingWave, edit->x(), edit->y());
}

void MainWindow::on_pushButtonEditWave_clicked()
{
    WindowWaveDesigner *wDesigner = new WindowWaveDesigner;
    wDesigner->show();
}

void MainWindow::on_actionAbout_triggered()
{
    DialogAbout *wAbout = new DialogAbout;
    wAbout->exec();
}

/*临时用作测试*/
void MainWindow::on_pushButtonStart_clicked()
{
    modu->clear();
    for (int i = 0; i < 10; i++)
    {
        modu->add(i/1.0, rand()%2);
    }
    update_myModuGraph();
    //ui->widgetModulatingWave->setGeometry(5, 10, 800, 200);
}

void MainWindow::on_pushButtonSend_clicked()
{
    //QVector<double> x(100000), y(100000);
    //update_waveGraph(ui->widgetModulatingWave, x, y);
}

void MainWindow::on_pushButtonConnect_clicked()
{
    //假装连接成功
    MainWindow::connect_success();
}
