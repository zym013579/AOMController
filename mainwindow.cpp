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
    m_serialPort(new QSerialPort)
{
    ui->setupUi(this);
    emit initWaveGraph(ui->widgetModulatingWave);
    emit initWaveGraph(ui->widgetEditingWave);

    emit closePort();
    emit searchDevice();

    connect(wDesigner, SIGNAL(sendWaveData(WaveData*)), this, SLOT(recieveWaveData(WaveData*)));
    connect(m_serialPort, SIGNAL(readyRead()), this, SLOT(recieveDeviceInfo()));
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

void initWaveGraph(QCustomPlot *target)
{
    target->clearGraphs();
    target->addGraph();
    target->yAxis->setRange(-0.2, 1.2);
    target->graph()->setLineStyle(QCPGraph::LineStyle::lsStepLeft); //方波图形
    target->replot();
}

void updateWaveGraph(QCustomPlot *target, QList<double> x, QList<double> y)
{
    target->graph(0)->setData(x.toVector(), y.toVector());
    target->replot();
}

void MainWindow::updateModuGraph()
{
    ui->widgetModulatingWave->xAxis->setRange(0, modu->x_at(modu->count()-1));
    emit updateWaveGraph(ui->widgetModulatingWave, modu->x(), modu->y());
}

void MainWindow::updateEditGraph()
{
    ui->widgetEditingWave->xAxis->setRange(0, edit->x_at(edit->count()-1));
    emit updateWaveGraph(ui->widgetEditingWave, edit->x(), edit->y());
}

void MainWindow::recieveWaveData(WaveData *data)
{
    edit->copyData(data);
    emit updateEditGraph();
}

bool MainWindow::openPort()
{
    if (m_serialPort->isOpen())
        return true;
    if (ui->comboBoxPort->currentIndex() == 0)
    {
        MSG_WARNING("没选端口");
        return false;
    }

    m_serialPort->setPortName(ui->comboBoxPort->currentText());
    if(!m_serialPort->open(QIODevice::ReadWrite))//用ReadWrite 的模式尝试打开串口
    {
        closePort();
        MSG_WARNING("打开失败");
        return false;
    }
    emit setPortConfig();
    ui->pushButtonStart->setEnabled(true);
    ui->pushButtonSend->setEnabled(true);
    ui->pushButtonConnect->setText("断开连接");
    ui->comboBoxPort->setEnabled(false);
    LINEEDIT_INFO("已连接");
    return true;
}

void MainWindow::closePort()
{
    if (m_serialPort->isOpen())
    {
        m_serialPort->clear();
        m_serialPort->close();
    }
    ui->pushButtonStart->setEnabled(false);
    ui->pushButtonSend->setEnabled(false);
    ui->pushButtonConnect->setText("连接下位机");
    ui->comboBoxPort->setEnabled(true);
    LINEEDIT_INFO("连接断开");
}

bool MainWindow::connectionTest()
{
    if (!openPort())
    {
        MSG_WARNING("端口未打开");
        return false;
    }

    m_serialPort->write("W_TTL");
    m_serialPort->waitForBytesWritten(3000);
    if (!m_serialPort->waitForReadyRead(3000))
    {
        closePort();
        MSG_WARNING("连接超时");
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

void MainWindow::modulating()
{
    ;
}

void MainWindow::modulate_interrupted()
{
    ;
}

void MainWindow::searchDevice()
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

void MainWindow::recieveDeviceInfo()
{
    ;
}

void MainWindow::on_pushButtonEditWave_clicked()
{
    wDesigner->recieveWaveData(edit);
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
        LINEEDIT_INFO("已开始调制");
        ui->pushButtonStart->setText("暂停调制");
    }
    else
    {
        ui->pushButtonStart->setText("开始调制");
    }
}

void MainWindow::on_pushButtonSend_clicked()
{
    if (!m_serialPort->isOpen() && !connectionTest())
    {
        emit closePort();
        MSG_WARNING("下位机连接失败，自动断开连接");
    }
    QByteArray sendInfo; //要发送的数据

    //数据处理部分未完成
    sendInfo = "\x00";

    m_serialPort->write(sendInfo, 1);
    if (!m_serialPort->waitForBytesWritten(5000))
    {
        LINEEDIT_INFO("发送超时");
        MSG_WARNING("发送超时");
    }
    else
    {
        LINEEDIT_INFO("已发送");
    }
}

void MainWindow::on_pushButtonConnect_clicked()
{
    if (ui->pushButtonConnect->text() == "连接下位机")
    {
        if (!openPort())
        {
            MSG_WARNING("打开端口失败");
            return;
        }
        if (!connectionTest())
        {
            closePort();
            MSG_WARNING("连接下位机失败");
            return;
        }
        //尝试连接
    }
    else
    {
        emit closePort();
    }
}

bool MainWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
    QString tmp = ui->comboBoxPort->currentText();
    if (eventType == "windows_generic_MSG")
    {
        MSG* msg = reinterpret_cast<MSG*>(message);
        //PDEV_BROADCAST_HDR lpdb = (PDEV_BROADCAST_HDR)msg->lParam;
        if (msg->message == WM_DEVICECHANGE && msg->wParam >= DBT_DEVICEARRIVAL)
        {
            emit searchDevice();
        }
    }
    if (tmp != "(未指定)" && ui->comboBoxPort->currentText() == "(未指定)")
    {
        closePort();
        MSG_WARNING("设备连接丢失");
    }
    Q_UNUSED(result);
    //return QWidget::nativeEvent(eventType, message, result);    //???
    return false;
}
