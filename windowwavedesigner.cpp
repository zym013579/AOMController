#include "windowwavedesigner.h"
#include "ui_windowwavedesigner.h"

WindowWaveDesigner::WindowWaveDesigner(QWidget *parent) :
    QMainWindow(parent),
    realTimeQuantify(DEFAULT_REALTIME_QUANTIFY),
    minDeltaTime(DEFAULT_MIN_DELTA_TIME),
    minDeltaVoltage(1.0/DEFAULT_VOL_QUANTIFY_LEVEL),
    ui(new Ui::WindowWaveDesigner),
    edit(new WaveData),
    mouseHasMoved(false),
    c_point(-1)    //选中已选择的点时改变值

{
    ui->setupUi(this);

    //设置可输入字符
    QRegExp regExpUDouble("(\\d*)(\\.\\d+)?");
    QRegExp regExpUInt("(\\d*)");
    ui->lineEditFrequency->setValidator(new QRegExpValidator(regExpUDouble, ui->lineEditFrequency));
    ui->lineEditPointTime->setValidator(new QRegExpValidator(regExpUDouble, ui->lineEditPointTime));
    ui->lineEditMaxVoltage->setValidator(new QRegExpValidator(regExpUDouble, ui->lineEditMaxVoltage));
    ui->lineEditPointNumber->setValidator(new QRegExpValidator(regExpUInt, ui->lineEditPointNumber));
    ui->lineEditPointVoltage->setValidator(new QRegExpValidator(regExpUDouble, ui->lineEditPointVoltage));

    emit initWaveGraph(ui->widgetWave);
    //edit->setDisX(MIN_X_DIS);
    //edit->setDisY(MIN_Y_DIS);

    ui->widgetWave->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ScatterShape::ssCircle, POINT_CIRCLE_SIZE));
    ui->widgetWave->setInteractions(QCP::iRangeDrag|QCP::iRangeZoom);// QCP::iSelectPlottables  QCP::iRangeZoom
    ui->widgetWave->graph()->setSelectable(QCP::SelectionType::stDataRange);   //仅可选择一个点QCP::SelectionType::stSingleData
    ui->widgetWave->axisRect()->setRangeDrag(Qt::Horizontal);   //设置仅水平拖动和缩放
    ui->widgetWave->axisRect()->setRangeZoom(Qt::Horizontal);

    connect(ui->actionUndo, SIGNAL(triggered()), this, SLOT(undoStep()));
    connect(ui->actionRedo, SIGNAL(triggered()), this, SLOT(redoStep()));
}

WindowWaveDesigner::~WindowWaveDesigner()
{
    delete edit;
    delete ui;
}

double checkData(double input, double min, double max)
{
    return MAX(min, MIN(max, input));
}

void WindowWaveDesigner::updateGraph()
{
    emit updateWaveGraph(ui->widgetWave, edit->x(), edit->y());
}

void WindowWaveDesigner::updateLineEditText()
{
    int i = witchPointclicked();
    if (i == -1)
    {
        ui->lineEditPointNumber->setText("");
        ui->lineEditPointTime->setText("");
        ui->lineEditPointVoltage->setText("");
    }
    else
    {
        ui->lineEditPointNumber->setText(QString::number(i));
        ui->lineEditPointTime->setText(QString::number(edit->x_at(i)));
        ui->lineEditPointVoltage->setText(QString::number(100*edit->y_at(i)));
    }
}

void WindowWaveDesigner::saveStep()
{
    edit->save();
    emit freshUndoRedoButton();
}

void WindowWaveDesigner::dropStep()
{
    edit->drop();
    emit updateGraph();
    emit updateLineEditText();
}

void WindowWaveDesigner::undoStep()
{
    if (edit->count_unDo() <= 1)
        return;
    edit->unDo();
    emit freshUndoRedoButton();
    emit choosePoint(-1);
}

void WindowWaveDesigner::redoStep()
{
    if (edit->count_reDo() < 1)
        return;
    edit->reDo();
    emit freshUndoRedoButton();
    emit choosePoint(-1);
}

void WindowWaveDesigner::freshUndoRedoButton()
{
    if (edit->count_unDo() > 1)
        ui->actionUndo->setEnabled(true);
    else
        ui->actionUndo->setEnabled(false);

    if (edit->count_reDo() > 0)
        ui->actionRedo->setEnabled(true);
    else
        ui->actionRedo->setEnabled(false);
}

int WindowWaveDesigner::witchPointclicking(QMouseEvent *event)
{
    for (int i=0; i<edit->count(); i++)
    {
        if ((X_VAL-edit->x_at(i))*X_RANGE > 1)
            continue;
        else if ((X_VAL-edit->x_at(i))*X_RANGE*(X_VAL-edit->x_at(i))*X_RANGE + (Y_VAL-edit->y_at(i))*Y_RANGE*(Y_VAL-edit->y_at(i))*Y_RANGE <= 1)
            return i;
        else if ((X_VAL-edit->x_at(i))*X_RANGE < -1)
            break;
    }
    return -1;
}

int WindowWaveDesigner::witchPointclicked()
{
    if (ui->widgetWave->graph()->selection() == QCPDataSelection())
        return -1;
    return ui->widgetWave->graph()->selection().dataRange().begin();
}

void WindowWaveDesigner::choosePoint(int i, int j)
{
    if (i <= -1)
        ui->widgetWave->graph()->setSelection(QCPDataSelection());
    else if (j == -1)
        ui->widgetWave->graph()->setSelection(QCPDataSelection(QCPDataRange(i, i+1)));
    else
        ui->widgetWave->graph()->setSelection(QCPDataSelection(QCPDataRange(i, j+1)));
    emit updateGraph();
    emit updateLineEditText();
}

void WindowWaveDesigner::recieveWaveData(WaveData *data)
{
    edit->clear();
    edit->setRealTimeQuantify(realTimeQuantify);
    edit->quantify(minDeltaTime, minDeltaVoltage);
    edit->copyData(data);
    emit freshUndoRedoButton();
    if (edit->count() > 1)
        ui->widgetWave->xAxis->setRange(-0.05*edit->x_at(edit->count()-1), 1.05*edit->x_at(edit->count()-1));
    else
        ui->widgetWave->xAxis->setRange(-0.25, 20.2);  //tmp
    emit choosePoint(-1);
}

void WindowWaveDesigner::on_widgetWave_mousePress(QMouseEvent *event)
{
    mouseHasMoved = false;
    c_point = -1;
    if (witchPointclicking(event) == witchPointclicked())
        c_point = witchPointclicked();
}

void WindowWaveDesigner::on_widgetWave_mouseRelease(QMouseEvent *event)
{
    if (!mouseHasMoved)
    {
        emit choosePoint(witchPointclicking(event));
    }
    else if (c_point != -1)
        emit saveStep();
    c_point = -1;
    ui->widgetWave->setInteractions(QCP::iRangeDrag|QCP::iRangeZoom);
}

void WindowWaveDesigner::on_widgetWave_mouseMove(QMouseEvent *event)
{
    mouseHasMoved = true;
    if (c_point == -1)
        return;
    ui->widgetWave->setInteractions(QCP::iRangeZoom);

    double x_val;
    if (c_point == 0)
        x_val = 0;
    else if (c_point != edit->count()-1)
        x_val = checkData(X_VAL, edit->x_at(c_point-1)+minDeltaTime, edit->x_at(c_point+1)-minDeltaTime);
    else
        x_val = MAX(edit->x_at(c_point-1)+minDeltaTime, X_VAL);
    edit->set(c_point, x_val, checkData(Y_VAL, 0, 1));
    emit updateGraph();
    emit updateLineEditText();
}

void WindowWaveDesigner::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton result = QMessageBox::information(this, "提示", "波形尚未应用，是否应用波形？", QMessageBox::Yes|QMessageBox::Save|QMessageBox::Discard|QMessageBox::Cancel, QMessageBox::Cancel);
    //event->accept();
    //event->ignore();
    if (result == QMessageBox::Yes)
        emit on_pushButtonSave_clicked();
    else if (result == QMessageBox::Save)
        ;
    //else if (result == QMessageBox::Discard) ;
    else if (result == QMessageBox::Cancel)
        event->ignore();
    return;
}

void WindowWaveDesigner::on_pushButtonNew_clicked()
{
    edit->add(edit->x_at(edit->count()-1)+DEFAULT_X_DIS, edit->y_at(edit->count()-1));
    emit saveStep();
    emit updateGraph();
}

void WindowWaveDesigner::on_pushButtonInsert_clicked()
{
    int i = witchPointclicked();
    if (i == -1 || i == edit->count()-1)
        emit on_pushButtonNew_clicked();
    else if (edit->x_at(i+1)-edit->x_at(i) > 10*minDeltaTime)
        edit->insert(i+1, (edit->x_at(i+1)+edit->x_at(i))/2.0, edit->y_at(i));
    else
        MSG_WARNING("两点间距过小，不能插入");
    emit saveStep();
    emit updateGraph();
}

void WindowWaveDesigner::on_pushButtonSave_clicked()
{
    emit sendWaveData(edit);
    // test部分
    //choosePoint(ui->lineEditPointNumber->text().toInt(), ui->lineEditFrequency->text().toInt());
    //qDebug() << ui->widgetWave->graph()->data()->at(ui->widgetWave->graph()->dataCount()-1)->key/(ui->widgetWave->graph()->dataCount()-1);
}

void WindowWaveDesigner::on_pushButtonDelete_clicked()
{
    if (witchPointclicked() == 0)
        MSG_WARNING("暂不支持删除起始点"); //不能删除初始点，或者删除后重排列
    else if (witchPointclicked() != -1)
    {
        edit->del(witchPointclicked());
        emit saveStep();
        c_point = -1;
        emit choosePoint(-1);
    }
    emit updateGraph();
}

void WindowWaveDesigner::on_lineEditPointNumber_textEdited(const QString &arg1)
{
    if (arg1 != "")
    {
        if (arg1.toInt() < edit->count() && arg1.toInt() >= 0)
        {
            emit choosePoint(arg1.toInt());
        }
        else
            emit choosePoint(-1);
    }
    else
    {
        emit choosePoint(-1);
    }
}

void WindowWaveDesigner::on_lineEditPointTime_textEdited(const QString &arg1)
{
    int i = witchPointclicked();
    if (i == -1 || arg1 == "" || i == 0)
        return;
    if (i == edit->count()-1)
        edit->set_x(i, MAX(edit->x_at(i-1), arg1.toDouble()));
    else
        edit->set_x(i, checkData(arg1.toDouble(), edit->x_at(i-1)+minDeltaTime, edit->x_at(i+1)-minDeltaTime));
    emit updateGraph();
}

void WindowWaveDesigner::on_lineEditPointVoltage_textEdited(const QString &arg1)
{
    int i = witchPointclicked();
    if (i == -1 || arg1 == "")
        return;
    edit->set_y(i, checkData(arg1.toDouble()/100, 0, 1));
    emit updateGraph();
}

void WindowWaveDesigner::on_lineEditPointNumber_editingFinished()
{
    if (ui->lineEditPointNumber->text().toInt() >= edit->count()) //序号部分
    {
        emit choosePoint(-1);
        MSG_WARNING("点序号超出范围");
        return;
    }
    emit saveStep();
}

void WindowWaveDesigner::on_lineEditPointTime_editingFinished()
{
    if (ui->lineEditPointTime->text() != "" && ui->lineEditPointNumber->text() == "")
    {
        emit dropStep();
        MSG_WARNING("没有选点");
        return;
    }
    int i = ui->lineEditPointNumber->text().toInt();
    if (i == 0) //时间部分
    {
        if (ui->lineEditPointTime->text().toDouble() != 0)
        {
            emit dropStep();
            MSG_WARNING("暂不能更改起始点的时间");
        }
        return; //由于起始点时间总是为0，不需要保存，直接返回
    }
    if (ui->lineEditPointTime->text().toDouble() < edit->x_at(i-1)+minDeltaTime)
    {
        emit dropStep();
        MSG_WARNING("点时间过小");
        return;
    }
    if (i < edit->count()-1 && ui->lineEditPointTime->text().toDouble() > edit->x_at(i+1)-minDeltaTime)
    {
        emit dropStep();
        MSG_WARNING("点时间过大");
        return;
    }
    emit updateLineEditText();
    emit saveStep();
}

void WindowWaveDesigner::on_lineEditPointVoltage_editingFinished()
{
    if (ui->lineEditPointVoltage->text() != "" && ui->lineEditPointNumber->text() == "")
    {
        emit dropStep();
        MSG_WARNING("没有选点");
        return;
    }
    if (ui->lineEditPointVoltage->text().toDouble() > 100)
    {
        emit dropStep();
        MSG_WARNING("点电压超出范围");
        return;
    }
    emit updateLineEditText();
    emit saveStep();
}
