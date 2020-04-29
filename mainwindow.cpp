#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    wDesigner(new WindowWaveDesigner),
    wAbout(new DialogAbout),
    wSetting(new DialogSetting),
    modu(new WaveData),
    edit(new WaveData),
    m_serialPortName({}),
    m_serialPort(new QSerialPort),
    connectError(false),
    realTimeQuantify(DEFAULT_REALTIME_QUANTIFY),
    volQuantiLevel(DEFAULT_VOL_QUANTIFY_LEVEL),
    minDeltaTime(DEFAULT_MIN_DELTA_TIME)
{
    ui->setupUi(this);

    emit defaultSettingsInit();
    emit initWaveGraph(ui->widgetModulatingWave);
    emit initWaveGraph(ui->widgetEditingWave);

    emit closePort();
    emit searchDevice();

    connect(wDesigner, SIGNAL(sendWaveData(WaveData*)), this, SLOT(recieveWaveDataFromEditor(WaveData*)));
    connect(wSetting, SIGNAL(sendSettings()), this, SLOT(recieveSettings()));
    //connect(m_serialPort, SIGNAL(readyRead()), this, SLOT(recieveDeviceInfo()));
}

MainWindow::~MainWindow()
{
    closePort();
    delete m_serialPort;
    delete modu;
    delete edit;
    delete wSetting;
    delete wAbout;
    delete wDesigner;
    delete ui;
}

void MainWindow::defaultSettingsInit()
{
    wDesigner->realTimeQuantify = realTimeQuantify;    //传递信息
    wDesigner->minDeltaTime = minDeltaTime;
    wDesigner->minDeltaVoltage = 1.0/volQuantiLevel;
    ui->lineEditVoltageStatus->setText(QString::number(volQuantiLevel));
    ui->lineEditFrequencyStatus->setText(QString::number(minDeltaTime));
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

void MainWindow::recieveWaveDataFromEditor(WaveData *data)
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

void MainWindow::setPortConfig()
{
    m_serialPort->setBaudRate(QSerialPort::Baud115200, QSerialPort::AllDirections);//设置波特率和读写方向
    m_serialPort->setDataBits(QSerialPort::Data8);		//数据位为8位
    m_serialPort->setFlowControl(QSerialPort::NoFlowControl);//无流控制
    m_serialPort->setParity(QSerialPort::NoParity);	//无校验位
    m_serialPort->setStopBits(QSerialPort::OneStop); //一位停止位
}

void MainWindow::searchDevice()
{
    m_serialPortName.clear();
    foreach(const QSerialPortInfo &info,QSerialPortInfo::availablePorts())
    {
        m_serialPortName << info.portName();
    }
    m_serialPortName.sort(Qt::CaseSensitivity::CaseInsensitive);
    int num = m_serialPortName.indexOf(ui->comboBoxPort->currentText())+1;
    ui->comboBoxPort->clear();
    ui->comboBoxPort->addItem("(未指定)");
    ui->comboBoxPort->addItems(m_serialPortName);
    ui->comboBoxPort->setCurrentIndex(num);
}

bool MainWindow::recieveDeviceInfo(int timeout)
{
    QByteArray recv = m_serialPort->readAll();
    if (!m_serialPort->waitForReadyRead(timeout))
    {
        closePort();
        MSG_WARNING("接收信息超时，关闭端口");
        return false;
    }
    if (recv.count() > RECV_MAX_LENGTH || recv.count() < RECV_MIN_LENGTH || recv != RECV_OK)
        return false;
    return true;
}

bool MainWindow::sendToDevice(QByteArray *data, int length, int timeout)
{
    if (!m_serialPort->isOpen())
    {
        MSG_WARNING("端口未打开");
        return false;
    }
    if (length == -1)
        m_serialPort->write(*data);
    else
        m_serialPort->write(*data, length);
    if (!m_serialPort->waitForBytesWritten(timeout))
    {
        LINEEDIT_INFO("发送超时");
        MSG_WARNING("发送超时");
        return false;
    }
    LINEEDIT_INFO("已发送");
    return true;
}

bool MainWindow::sendCommandToDevice(QString command)
{
    if (command == "start")
    {
        QByteArray msg = SEND_START;
        if (!sendToDevice(&msg))
            return false;
        if (!recieveDeviceInfo())
        {
            MSG_WARNING("开始失败");
            return false;
        }
        return true;
    }
    else if (command == "pause")
    {
        QByteArray msg = SEND_PAUSE;
        if (!sendToDevice(&msg))
            return false;
        if (!recieveDeviceInfo())
        {
            MSG_WARNING("暂停失败");
            return false;
        }
        return true;
    }
    else if (command == "test")
    {
        QByteArray msg = SEND_CONNECTION_TEST;
        if (!sendToDevice(&msg))
            return false;
        if (!recieveDeviceInfo())
        {
            MSG_WARNING("连接出现错误");
            return false;
        }
        return true;
    }
    else
    {
        MSG_WARNING("命令错误");
        return false;
    }
}

void MainWindow::recieveSettings(bool rTQ, int vQL, double mDT)
{
    realTimeQuantify = rTQ;
    volQuantiLevel = vQL;
    minDeltaTime = mDT;
    ui->lineEditVoltageStatus->setText(QString::number(volQuantiLevel));
    ui->lineEditFrequencyStatus->setText(QString::number(minDeltaTime));
}

void MainWindow::on_pushButtonEditWave_clicked()
{
    wDesigner->realTimeQuantify = realTimeQuantify;    //传递信息
    wDesigner->minDeltaTime = minDeltaTime;
    wDesigner->minDeltaVoltage = 1.0/volQuantiLevel;
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
        LINEEDIT_INFO("已暂停调制");
        ui->pushButtonStart->setText("开始调制");
    }
}

void MainWindow::on_pushButtonSend_clicked()
{
    QByteArray sendInfo; //要发送的数据

    //数据处理部分未完成
    sendInfo = "\x00";

    if (!sendToDevice(&sendInfo, 1))
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
        if (!sendCommandToDevice("test"))
        {
            closePort();
            MSG_WARNING("连接下位机失败");
            return;
        }
    }
    else
    {
        emit closePort();
    }
}

void MainWindow::on_actionSetting_triggered()
{
    wSetting->exec();
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

void initWaveGraph(QCustomPlot *target)
{
    target->clearGraphs();
    target->addGraph();
    target->yAxis->setRange(-0.2, 1.2);
    target->xAxis->setRange(-0.2, 20.2);
    target->graph()->setLineStyle(QCPGraph::LineStyle::lsStepLeft); //方波图形
    target->replot();
}

void updateWaveGraph(QCustomPlot *target, QList<double> x, QList<double> y)
{
    target->graph(0)->setData(x.toVector(), y.toVector());
    target->replot();
}

