#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    wDesigner(new WindowWaveDesigner),
    wAbout(new DialogAbout),
    modu(new WaveData),
    edit(new WaveData),
    m_serialPortName({}),
    m_serialPort(new QSerialPort),
    m_connectStatus(false)
{
    ui->setupUi(this);
    emit init_waveGraph(ui->widgetModulatingWave);
    emit init_waveGraph(ui->widgetEditingWave);

    emit closePort();

    emit search_device();

    connect(wDesigner, SIGNAL(send_waveData(WaveData*)), this, SLOT(recieve_waveData(WaveData*)));

    connect(m_serialPort, SIGNAL(readyRead()), this, SLOT(recieveInfo()));
}

MainWindow::~MainWindow()
{
    closePort();
    delete m_serialPort;
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

bool MainWindow::openPort()
{
    if (!m_serialPort->isOpen())
        return true;
    if (ui->comboBoxPort->currentIndex() == 0)
    {
        AAA("没选端口");
        return false;
    }

    m_serialPort->setPortName(ui->comboBoxPort->currentText());
    if(!m_serialPort->open(QIODevice::ReadWrite))//用ReadWrite 的模式尝试打开串口
    {
        AAA("打开失败");
        return false;
    }
    emit setPortConfig();
    return true;
}

void MainWindow::closePort()
{
    if (m_serialPort->isOpen())
    {
        m_serialPort->clear();
        m_serialPort->close();
    }
}

bool MainWindow::try_toConnect()
{
    if (!openPort())
        return false;

    m_serialPort->write(W_TTL);
    m_serialPort->waitForBytesWritten(3000);
    if (!m_serialPort->waitForReadyRead(3000))
    {
        AAA("连接超时");
        closePort();
        return false;
    }
    return true;
}

void MainWindow::setPortConfig()
{
    m_serialPort->setBaudRate(QSerialPort::Baud115200,QSerialPort::AllDirections);//设置波特率和读写方向
    m_serialPort->setDataBits(QSerialPort::Data8);		//数据位为8位
    m_serialPort->setFlowControl(QSerialPort::NoFlowControl);//无流控制
    m_serialPort->setParity(QSerialPort::NoParity);	//无校验位
    m_serialPort->setStopBits(QSerialPort::OneStop); //一位停止位
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

void MainWindow::search_device()
{
    m_serialPortName.clear();
    foreach(const QSerialPortInfo &info,QSerialPortInfo::availablePorts())
    {
        m_serialPortName << info.portName();
        //qDebug()<<"serialPortName:"<<info.portName();
    }
    m_serialPortName.sort(Qt::CaseSensitivity::CaseInsensitive);
    int num = m_serialPortName.indexOf(ui->comboBoxPort->currentText())+1;
    ui->comboBoxPort->clear();
    ui->comboBoxPort->addItem("(未指定)");
    ui->comboBoxPort->addItems(m_serialPortName);
    ui->comboBoxPort->setCurrentIndex(num);
}

void MainWindow::recieveInfo()
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
        if (try_toConnect())
        {
            emit connected();
            return;
        }
        AAA("连接失败");
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
            //QMessageBox::information(this, "test", "test");
            emit search_device();
        }
    }
    Q_UNUSED(result);
    //return QWidget::nativeEvent(eventType, message, result);    //???
    return false;
}
