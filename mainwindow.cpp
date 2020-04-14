#include "mainwindow.h"
#include "ui_mainwindow.h"

//QVector<double> WaveData::editX, WaveData::editY, WaveData::moduX, WaveData::moduY, WaveData::tempX, WaveData::tempY;
//double WaveData::deltaX, WaveData::voltage;
//WaveData WaveData;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    wDesigner(new WindowWaveDesigner),
    wAbout(new DialogAbout),
    modu(new WaveData),
    edit(new WaveData)
{
    ui->setupUi(this);
    emit init_waveGraph(ui->widgetModulatingWave);
    emit init_waveGraph(ui->widgetEditingWave);

    connect(wDesigner, SIGNAL(send_waveData(WaveData*)), this, SLOT(recieve_waveData(WaveData*)));
}

MainWindow::~MainWindow()
{
    delete modu;
    delete edit;
    delete wAbout;
    delete wDesigner;
    delete ui;
}

void MainWindow::connect_success()
{
    ui->pushButtonStart->setEnabled(true);
    ui->pushButtonSend->setEnabled(true);
    ui->lineEditConnectStatus->setText("已连接，未调制");
}

void init_waveGraph(QCustomPlot *target)
{
    target->clearGraphs();
    target->addGraph();
    target->yAxis->setRange(-0.2, 1.2);
    target->graph()->setLineStyle(QCPGraph::LineStyle::lsStepLeft); //方波图形
    target->replot();
}

void update_waveGraph(QCustomPlot *target, QList<double> x, QList<double> y)
{
    target->graph(0)->setData(x.toVector(), y.toVector());
    target->replot();
}

/* 自动设置显示范围
void reset_waveGraphAxis(QCustomPlot *target)
{
    //target->rescaleAxes();
    qDebug() << target->graph();
    qDebug() << target->graph()->data()->end()->key;
    target->xAxis->setRange(0, MAX(5, target->graph()->data()->end()->value));
    target->replot();
}*/

void MainWindow::update_myModuGraph()
{
    ui->widgetModulatingWave->xAxis->setRange(0, modu->x_at(modu->count()-1));
    emit update_waveGraph(ui->widgetModulatingWave, modu->x(), modu->y());
}

void MainWindow::update_myEditGraph()
{
    ui->widgetEditingWave->xAxis->setRange(0, edit->x_at(edit->count()-1));
    emit update_waveGraph(ui->widgetEditingWave, edit->x(), edit->y());
}

void MainWindow::recieve_waveData(WaveData *data)
{
    edit->copyData(data);
    emit update_myEditGraph();
}

void MainWindow::on_pushButtonEditWave_clicked()
{
    wDesigner->recieve_waveData(edit);
    wDesigner->show();
}

void MainWindow::on_actionAbout_triggered()
{
    wAbout->exec();
}

/*暂时用于测试其他内容*/
void MainWindow::on_pushButtonStart_clicked()
{
    edit->clear();
    for (int i = 0; i < 10; i++)
    {
        edit->add(i/1.0, rand()%2);
    }
    emit update_myEditGraph();
}

void MainWindow::on_pushButtonSend_clicked()
{
    ;
}

void MainWindow::on_pushButtonConnect_clicked()
{
    //假装连接成功
    emit connect_success();
}
