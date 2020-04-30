#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    modu(new WaveData),
    edit(new WaveData),
    m_serialPortName({}),
    m_serialPort(new QSerialPort),
    connectError(false)
{
    ui->setupUi(this);

    emit defaultSettingsInit();
    emit initWaveGraph(ui->widgetModulatingWave);
    emit initWaveGraph(ui->widgetEditingWave);

    SET_REALTIME_QUANTIFY(DEFAULT_REALTIME_QUANTIFY);
    SET_VOL_QUANTIFY_LEVEL(DEFAULT_VOL_QUANTIFY_LEVEL);
    SET_MIN_DELTA_TIME(DEFAULT_MIN_DELTA_TIME);

    emit closePort();
    emit searchDevice();

    //connect(wDesigner, SIGNAL(sendWaveData(WaveData*)), this, SLOT(recieveWaveDataFromEditor(WaveData*)));
    //connect(wSetting, SIGNAL(sendSettings(bool, int, double)), this, SLOT(recieveSettings(bool, int, double)));
    //connect(m_serialPort, SIGNAL(readyRead()), this, SLOT(recieveDeviceInfo()));
}

MainWindow::~MainWindow()
{
    closePort();
    delete m_serialPort;
    delete modu;
    delete edit;
    //delete wSetting;
    //delete wAbout;
    //delete wDesigner;
    delete ui;
}

void MainWindow::defaultSettingsInit()
{
    //wDesigner->realTimeQuantify = realTimeQuantify;    //传递信息
    //wDesigner->minDeltaTime = minDeltaTime;
    //wDesigner->minDeltaVoltage = 1.0/volQuantiLevel;
    //ui->lineEditVoltageStatus->setText(QString::number(volQuantiLevel));
    //ui->lineEditFrequencyStatus->setText(QString::number(minDeltaTime));
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
    SET_REALTIME_QUANTIFY(data->getRealTimeQuantify());
    SET_VOL_QUANTIFY_LEVEL(data->getVolQuantiLevel());
    SET_MIN_DELTA_TIME(data->getMinDeltaTime());
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
    //QByteArray recv = m_serialPort->readAll();
    if (!m_serialPort->waitForReadyRead(timeout))
    {
        closePort();
        MSG_WARNING("接收信息超时，关闭端口");
        return false;
    }
    QByteArray recv = m_serialPort->readAll();
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

//void MainWindow::recieveSettings(bool rTQ, int vQL, double mDT)
//{
//    SET_REALTIME_QUANTIFY(rTQ);
//    SET_VOL_QUANTIFY_LEVEL(vQL);
//    SET_MIN_DELTA_TIME(mDT);
//}

void MainWindow::on_pushButtonEditWave_clicked()
{
    WindowWaveDesigner *wDesigner = new WindowWaveDesigner(this);
    emit sendSettings(edit);
    //wDesigner->recieveWaveData(edit);
    connect(wDesigner, SIGNAL(sendWaveData(WaveData*)), this, SLOT(recieveWaveDataFromEditor(WaveData*)));
    //connect(wDesigner, SIGNAL(sendSettings(bool, int, double)), this, SLOT(recieveSettings(bool, int, double)));
    wDesigner->show();
}

void MainWindow::on_actionAbout_triggered()
{
    DialogAbout *wAbout = new DialogAbout;
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
    if (edit->count() < 2)
    {
        MSG_WARNING("点数量过少");
        return;
    }

    edit->quantify(MIN_DELTA_TIME, VOL_QUANTIFY_LEVEL); //量化数据
    edit->save();
    QByteArray sendInfo = ""; //要发送的数据

    sendInfo.append('{');
    for (int i = 0; i < edit->count()-1; i++)
    {
        int y = trunc(edit->y_at(i)*VOL_QUANTIFY_LEVEL);
//        if (y == VOL_QUANTIFY_LEVEL)    //防止下位机溢出，在级数较少时可能存在误差
//            y--;

//        if (y > 16383)
//        {
//            closePort();
//            MSG_WARNING("波形转换发生致命错误");
//            return;
//        }
        y = ((y/128)*256 + (y%128)) | 0x8080;
        //sendInfo.append(y);
//        t1.remove(1);
//        t1 = QString::number(((t1.toInt(Q_NULLPTR, 16)<<2)+t2.toInt()/8) | 8, 16);
//        t2 = QString::number(t2.toInt(Q_NULLPTR, 16) | 8);
//        sendInfo.append(t1);
//        sendInfo.append(t2);

        int dx = trunc((edit->x_at(i+1)-edit->x_at(i))/MIN_DELTA_TIME);

        while (dx > 16383)  //2^14-1
        {
            sendInfo.append(y/0x100);
            sendInfo.append(y%0x100);
            int times = dx/16384;
            if (times > 127)
                times = 127;
            sendInfo.append('M');
            sendInfo.append(times|0x80);
            dx = dx - 16384*times;
            sendInfo.append(';');
        }

        if (dx > 0)
        {
            sendInfo.append(y/0x100);
            sendInfo.append(y%0x100);
            dx = ((dx/128)*256 + (dx%128)) | 0x8080;
            sendInfo.append(dx/0x100);
            sendInfo.append(dx%0x100);
            sendInfo.append(';');
        }
    }
    sendInfo.append('}');

    if (!sendToDevice(&sendInfo))
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

//void MainWindow::on_actionSetting_triggered()
//{
//    DialogSetting *wSetting = new DialogSetting(this);
//    emit sendSettings(REALTIME_QUANTIFY, VOL_QUANTIFY_LEVEL, MIN_DELTA_TIME);
//    connect(wSetting, SIGNAL(sendSettings(bool, int, double)), this, SLOT(recieveSettings(bool, int, double)));
//    wSetting->exec();
//}

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

QString numberToStr(int num)
{
    return QString::number(num);
}

QString numberToStr(double num)
{
    QString t = QString::number(num, 'f');
    while (t.indexOf('.') != -1 && t.at(t.count()-1) == '0')
        t.remove(t.count()-1, 1);
    if (t.indexOf('.') == t.count()-1)
        t.remove(t.count()-1, 1);
    return t;
}
