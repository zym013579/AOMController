#include "mainwindow.h"
#include "ui_mainwindow.h"

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

int try_toConnect()
{
    return 0;
}

void search_device()
{
    ;
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

void MainWindow::connected()
{
    ui->pushButtonStart->setEnabled(true);
    ui->pushButtonSend->setEnabled(true);
    ui->pushButtonConnect->setText("断开连接");
    ui->lineEditConnectStatus->setText("已连接，暂停调制");
}

void MainWindow::disconnected()
{
    ui->pushButtonStart->setEnabled(false);
    ui->pushButtonSend->setEnabled(false);
    ui->pushButtonConnect->setText("连接下位机");
    ui->lineEditConnectStatus->setText("连接断开");
}

void MainWindow::modulating()
{
    ;
}

void MainWindow::modulate_interrupted()
{
    ;
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

void MainWindow::on_pushButtonStart_clicked()
{
    if (ui->pushButtonStart->text() == "开始调制")
    {
        ui->lineEditConnectStatus->setText("已连接，正在调制");
        ui->pushButtonStart->setText("暂停调制");
    }
    else
    {
        ui->pushButtonStart->setText("开始调制");
    }
}

void MainWindow::on_pushButtonSend_clicked()
{
    //假装发送成功
}

void MainWindow::on_pushButtonConnect_clicked()
{
    if (ui->pushButtonConnect->text() == "连接下位机")
    {
        //尝试连接
        if (1) //假装连接成功
            emit connected();
        else
        {
            ui->lineEditConnectStatus->setText("连接失败");
        }
    }
    else
    {
        //断开前操作
        emit disconnected();
    }
}

bool MainWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
    if (eventType == "windows_generic_MSG")
    {
        MSG* msg = reinterpret_cast<MSG*>(message);
        //PDEV_BROADCAST_HDR lpdb = (PDEV_BROADCAST_HDR)msg->lParam;
        if (msg->message == WM_DEVICECHANGE && msg->wParam >= DBT_DEVICEARRIVAL)
        {
            //search_device();
            QMessageBox::information(this, "test", "test");
        }
    }
    Q_UNUSED(result);
    //return QWidget::nativeEvent(eventType, message, result);    //???
    return false;
}
