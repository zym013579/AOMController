#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSerialPortInfo>

#include "windowwavedesigner.h"
#include "dialogabout.h"
#include "init.h"
#include "command.h"
#include <dbt.h>

//设置信息框的内容
#define LINEEDIT_INFO(message) (ui->lineEditConnectStatus->setText(message))

//设置各项文本框/复选框的显示内容
#define SETCHECKBOX_REALTIME_QUANTIFY(enabled) (ui->checkBoxRealTimeQuanti->setChecked(enabled))
#define SETTEXT_VOL_QUANTIFY_LEVEL(level) (ui->lineEditVoltageStatus->setText(numberToStr(level)))
#define SETTEXT_UNIT_TIME(time) (ui->lineEditFrequencyStatus->setText(numberToStr(time)))
#define SET_MODUTEXT_VOL_QUANTIFY_LEVEL(level) (ui->lineEditModuVolQuntiLevel->setText(numberToStr(level)))
#define SET_MODUTEXT_UNIT_TIME(time) (ui->lineEditModuUnitTime->setText(numberToStr(time)))

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    modu(new WaveData),
    edit(new WaveData),
    m_serialPortName({}),
    m_serialPort(new QSerialPort)
{
    ui->setupUi(this);

    emit initWaveGraph(ui->widgetModulatingWave);
    emit initWaveGraph(ui->widgetEditingWave);

    modu->setSaveMode(MODU_WAVE_MODE);
    edit->setSaveMode(EDIT_WAVE_MODE);
    emit freshUndoRedoButton();

    SETCHECKBOX_REALTIME_QUANTIFY(DEFAULT_REALTIME_QUANTIFY);
    SETTEXT_VOL_QUANTIFY_LEVEL(DEFAULT_VOL_QUANTIFY_LEVEL);
    SETTEXT_UNIT_TIME(DEFAULT_UNIT_TIME);

    emit closePort();
    emit searchDevice();
}

MainWindow::~MainWindow()
{
    closePort();
    delete m_serialPort;
    delete modu;
    delete edit;
    delete ui;
}

void MainWindow::updateModuGraph()
{
    SET_MODUTEXT_VOL_QUANTIFY_LEVEL(modu->getVolQuantiLevel());
    SET_MODUTEXT_UNIT_TIME(modu->getUnitTime());
    ui->widgetModulatingWave->xAxis->setRange(0, modu->xAt(modu->count()-1));
    emit updateWaveGraph(ui->widgetModulatingWave, modu->x(), modu->y(), modu->getMaxVol());
}

void MainWindow::updateEditGraph()
{
    SETCHECKBOX_REALTIME_QUANTIFY(edit->getRealTimeQuantify());
    SETTEXT_VOL_QUANTIFY_LEVEL(edit->getVolQuantiLevel());
    SETTEXT_UNIT_TIME(edit->getUnitTime());
    ui->widgetEditingWave->xAxis->setRange(0, edit->xAt(edit->count()-1));
    emit updateWaveGraph(ui->widgetEditingWave, edit->x(), edit->y(), edit->getMaxVol());
}

void MainWindow::recieveWaveDataFromEditor(WaveData *data)
{
    edit->copyData(data);
    emit freshUndoRedoButton();
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
    if(!m_serialPort->open(QIODevice::ReadWrite))//用ReadWrite的模式尝试打开串口
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
    /* 该部分可根据实际需要进行修改
     *
     * 条件语句识别命令，并作出相应操作
     */
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

void MainWindow::on_pushButtonEditWave_clicked()
{
    WindowWaveDesigner *wDesigner = new WindowWaveDesigner(this);
    emit sendSettings(edit);
    //准备接收波形
    connect(wDesigner, SIGNAL(sendWaveData(WaveData*)), this, SLOT(recieveWaveDataFromEditor(WaveData*)));
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
        if (sendCommandToDevice("start"))
        {
            LINEEDIT_INFO("已开始调制");
            ui->pushButtonStart->setText("暂停调制");
        }
        else
            LINEEDIT_INFO("开始失败");
    }
    else
    {
        if (sendCommandToDevice("pause"))
        {
            LINEEDIT_INFO("已暂停调制");
            ui->pushButtonStart->setText("开始调制");
        }
        else
            LINEEDIT_INFO("暂停失败");
    }
}

void MainWindow::on_pushButtonSend_clicked()
{
    if (edit->count() < 2)
    {
        MSG_WARNING("点数量过少");
        return;
    }

    /* 该部分可根据实际需求修改
     *
     * 该部分将edit中的波形信息转换后发送给下位机
     *
     * 目前QByteArray上限为9999字符，超过部分分多次发送
     * 可能会存在接收问题
     */

    edit->quantify(edit->getUnitTime(), edit->getVolQuantiLevel(), true); //量化数据
    edit->save();
    QList<QByteArray*> sendInfoAll;
    int num = 0;
    int bytenum = 0;
    sendInfoAll.append(new QByteArray);
    QByteArray *sendInfo = sendInfoAll[num]; //要发送的数据

    sendInfo->append('{');
    bytenum++;
    for (int i = 0; i < edit->count()-1; i++)
    {
        int y = qRound(edit->yAt(i)*(MAX_VOL_QUANTIFY_LEVEL*1.0)/(edit->getVolQuantiLevel()*1.0))*edit->getVolQuantiLevel();
        y = ((y/128)*256 + (y%128)) | 0x8080;

        int dx = trunc((edit->xAt(i+1)-edit->xAt(i))/edit->getUnitTime());

        while (dx > 16383)  //2^14-1
        {
            sendInfo->append(y/0x100);
            sendInfo->append(y%0x100);
            int times = dx/16384;
            if (times > 127)
                times = 127;
            sendInfo->append('M');
            sendInfo->append(times|0x80);
            dx = dx - 16384*times;
            sendInfo->append(';');
            bytenum += 5;
            if (bytenum > QByteArray_MAX_NUM-10)
            {
                num++;
                sendInfoAll.append(new QByteArray);
                sendInfo = sendInfoAll[num];
                bytenum = 0;
            }
        }

        if (dx > 0)
        {
            sendInfo->append(y/0x100);
            sendInfo->append(y%0x100);
            dx = ((dx/128)*256 + (dx%128)) | 0x8080;
            sendInfo->append(dx/0x100);
            sendInfo->append(dx%0x100);
            sendInfo->append(';');
            bytenum += 5;
            if (bytenum > QByteArray_MAX_NUM-10)
            {
                num++;
                sendInfoAll.append(new QByteArray);
                sendInfo = sendInfoAll[num];
                bytenum = 0;
            }
        }
    }
    sendInfo->append('}');

    bool isOk = true;
    for (int i = 0; i <= num; i++)
    {
        if (!isOk)
        {
            delete sendInfoAll[i];
            continue;
        }
        if (!sendToDevice(sendInfoAll[i]))
            isOk = false;
        Sleep(1200);    //传输延迟暂时解决办法
        delete sendInfoAll[i];
    }

    if (!isOk)
    {
        LINEEDIT_INFO("发送失败");
        MSG_WARNING("发送失败");
        return;
    }
    LINEEDIT_INFO("已发送");
    modu->clear();
    modu->copyData(edit);
    emit updateModuGraph();
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
        emit closePort();
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
    target->graph()->setLineStyle(QCPGraph::LineStyle::lsStepLeft); //方波图形，可根据实际修改
    target->replot();
}

void updateWaveGraph(QCustomPlot *target, QList<double> x, QList<double> y, double maxVol)
{
    for (int i = 0; i < y.count(); i++)
        y.replace(i, y.at(i)*maxVol);
    target->yAxis->setRange(-0.2*maxVol, 1.2*maxVol);
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

void MainWindow::on_actionPreWave_triggered()
{
    if (edit->countUnDo() < 1)
        return;
    edit->unDo();
    emit freshUndoRedoButton();
    emit updateEditGraph();
}

void MainWindow::freshUndoRedoButton()
{
    if (edit->countUnDo() > 0)
        ui->actionPreWave->setEnabled(true);
    else
        ui->actionPreWave->setEnabled(false);

    if (edit->countReDo() > 0)
        ui->actionNextWave->setEnabled(true);
    else
        ui->actionNextWave->setEnabled(false);
}

void MainWindow::on_actionNextWave_triggered()
{
    if (edit->countReDo() < 1)
        return;
    edit->reDo();
    emit freshUndoRedoButton();
    emit updateEditGraph();
}
