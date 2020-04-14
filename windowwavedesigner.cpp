#include "windowwavedesigner.h"
#include "ui_windowwavedesigner.h"

//QVector<double> WaveData::EditX, WaveData::EditY, WaveData::ModuX, WaveData::ModuY;
//bool notmove;

WindowWaveDesigner::WindowWaveDesigner(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WindowWaveDesigner),
    edit(new WaveData),
    mouseHasMoved(false),
    c_point(-1),    //选中已选择的点时改变值
    point_circleSize(13),    //图形中的点的大小
    disX(0.5),  //新建点默认间距
    minDisX(0.01)   //允许最短间距
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

    emit init_waveGraph(ui->widgetWave);

    ui->widgetWave->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ScatterShape::ssCircle, point_circleSize));
    ui->widgetWave->setInteractions(QCP::iRangeDrag|QCP::iRangeZoom);// QCP::iSelectPlottables  QCP::iRangeZoom
    ui->widgetWave->graph()->setSelectable(QCP::SelectionType::stSingleData);   //仅可选择一个点
    ui->widgetWave->axisRect()->setRangeDrag(Qt::Horizontal);   //设置仅水平拖动和缩放
    ui->widgetWave->axisRect()->setRangeZoom(Qt::Horizontal);

//    connect(ui->widgetWave, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(on_widgetWave_mousePress(QMouseEvent*)));
//    connect(ui->widgetWave, SIGNAL(mouseRelease(QMouseEvent*)), this, SLOT(on_widgetWave_mouseRelease(QMouseEvent*)));
//    connect(ui->widgetWave, SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(on_widgetWave_mouseMove(QMouseEvent*)));

//    connect(ui->lineEditPointNumber, SIGNAL(textEdited(QString)), this, SLOT(update_pointNumber(QString)));
//    connect(ui->lineEditPointTime, SIGNAL(textEdited(QString)), this, SLOT(update_pointData()));
//    connect(ui->lineEditPointVoltage, SIGNAL(textEdited(QString)), this, SLOT(update_pointData()));
//    connect(ui->lineEditPointNumber, SIGNAL(editingFinished()), this, SLOT(check_pointText()));
//    connect(ui->lineEditPointTime, SIGNAL(editingFinished()), this, SLOT(check_pointText()));
//    connect(ui->lineEditPointVoltage, SIGNAL(editingFinished()), this, SLOT(check_pointText()));

    connect(ui->actionUndo, SIGNAL(triggered()), this, SLOT(undo_step()));
    connect(ui->actionRedo, SIGNAL(triggered()), this, SLOT(redo_step()));
}

WindowWaveDesigner::~WindowWaveDesigner()
{
    delete edit;
    delete ui;
}

double check_data(double input, double min, double max)
{
    return MAX(min, MIN(max, input));
}

void WindowWaveDesigner::update_myGraph()
{
    emit update_waveGraph(ui->widgetWave, edit->x(), edit->y());
}

void WindowWaveDesigner::update_pointText()
{
    int i = witch_pointClicked();
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

/* 早期文本框编辑相关事件代码
void WindowWaveDesigner::check_pointText()
{
    int i = ui->lineEditPointNumber->text().toInt();
    if (i >= edit->count()) //序号部分
    {
        emit choose_point(-1);
        emit update_pointText();
        QMessageBox::warning(this, "注意", "点序号超出范围", QMessageBox::Ok, QMessageBox::Ok);
        return;
    }

    if (i == 0) //时间部分
    {
        if (ui->lineEditPointTime->text().toDouble() != 0)
        {
            emit drop_step();
            QMessageBox::warning(this, "注意", "暂不能更改起始点的时间", QMessageBox::Ok, QMessageBox::Ok);
            return;
        }
    }
    else
    {
        if (ui->lineEditPointTime->text().toDouble() < edit->x_at(i-1)+minDisX)
        {
            emit drop_step();
            QMessageBox::warning(this, "注意", "点时间过小", QMessageBox::Ok, QMessageBox::Ok);
            return;
        }
        if (i < edit->count()-1 && ui->lineEditPointTime->text().toDouble() > edit->x_at(i+1)-minDisX)
        {
            emit drop_step();
            QMessageBox::warning(this, "注意", "点时间过大", QMessageBox::Ok, QMessageBox::Ok);
            return;
        }
    }

    if (ui->lineEditPointVoltage->text().toDouble() > 100)
    {
        emit drop_step();
        QMessageBox::warning(this, "注意", "点电压超出范围", QMessageBox::Ok, QMessageBox::Ok);
        return;
    }
    emit save_step();
}

void WindowWaveDesigner::update_pointNumber()
{
    int i = ui->lineEditPointNumber->text().toInt();
    if (ui->lineEditPointNumber->text() != "")
    {
        if (i < edit->count() && i >= 0)
        {
            emit choose_point(i);
            emit update_pointText();
        }
        else
            emit choose_point(-1);
    }
    else
    {
        emit choose_point(-1);
        emit update_pointText();
    }
}

void WindowWaveDesigner::update_pointData()
{
    if (witch_pointClicked() == -1 || ui->lineEditPointTime->text() == "" || ui->lineEditPointVoltage->text() == "")
        return;
    int i = ui->lineEditPointNumber->text().toInt();
    if (i == 0)
        edit->set(0, 0, check_data(ui->lineEditPointVoltage->text().toDouble()/100, 0, 1));
    else if (i == edit->count()-1)
        edit->set(i, MAX(edit->x_at(i-1), ui->lineEditPointTime->text().toDouble()), check_data(ui->lineEditPointVoltage->text().toDouble()/100, 0, 1));
    else
        edit->set(i, check_data(ui->lineEditPointTime->text().toDouble(), edit->x_at(i-1)+minDisX, edit->x_at(i+1)-minDisX), check_data(ui->lineEditPointVoltage->text().toDouble()/100, 0, 1));
    emit update_myGraph();
}
*/

void WindowWaveDesigner::save_step()
{
    edit->save();
    emit fresh_undo_redo_button();
}

void WindowWaveDesigner::drop_step()
{
    edit->drop();
    emit update_myGraph();
    emit update_pointText();
}

void WindowWaveDesigner::undo_step()
{
    if (edit->count_unDo() <= 1)
        return;
    edit->unDo();
    emit fresh_undo_redo_button();
    emit choose_point(-1);
    emit update_pointText();
    emit update_myGraph();
    emit update_pointText();
}

void WindowWaveDesigner::redo_step()
{
    if (edit->count_reDo() < 1)
        return;
    edit->reDo();
    emit fresh_undo_redo_button();
    emit choose_point(-1);
    emit update_pointText();
    emit update_myGraph();
    emit update_pointText();
}

void WindowWaveDesigner::fresh_undo_redo_button()
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

int WindowWaveDesigner::if_pointClicked(QMouseEvent *event)
{
    for (int i=0; i<edit->count(); i++)
    {
        if ((X_VAL-edit->x_at(i))*X_RANGE > 1)
            continue;
        else if (qPow((X_VAL-edit->x_at(i))*X_RANGE, 2)+qPow((Y_VAL-edit->y_at(i))*Y_RANGE, 2) <= 1)
            return i;
        else if ((X_VAL-edit->x_at(i))*X_RANGE < -1)
            break;
    }
    return -1;
}

int WindowWaveDesigner::witch_pointClicked()
{
    if (ui->widgetWave->graph()->selection() == QCPDataSelection())
        return -1;
    return ui->widgetWave->graph()->selection().dataRange().begin();
}

void WindowWaveDesigner::choose_point(int i)
{
    if (i > -1)
        ui->widgetWave->graph()->setSelection(QCPDataSelection(QCPDataRange(i, i+1)));
    else
        ui->widgetWave->graph()->setSelection(QCPDataSelection());
    emit update_myGraph();
}

void WindowWaveDesigner::recieve_waveData(WaveData *data)
{
    edit->clear();
    edit->copyData(data);
    emit fresh_undo_redo_button();
    if (edit->count() > 1)
        ui->widgetWave->xAxis->setRange(-0.05*edit->x_at(edit->count()-1), 1.05*edit->x_at(edit->count()-1));
    else
        ui->widgetWave->xAxis->setRange(-0.25, 5);  //tmp
    emit update_myGraph();
    emit choose_point(-1);
    emit update_pointText();
}

void WindowWaveDesigner::on_widgetWave_mousePress(QMouseEvent *event)
{
    mouseHasMoved = false;
    c_point = -1;
    if (if_pointClicked(event) == witch_pointClicked())
        c_point = witch_pointClicked();
}

void WindowWaveDesigner::on_widgetWave_mouseRelease(QMouseEvent *event)
{
    if (!mouseHasMoved)
    {
        emit choose_point(if_pointClicked(event));
        emit update_pointText();
    }
    else if (c_point != -1)
        emit save_step();
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
        x_val = check_data(X_VAL, edit->x_at(c_point-1)+minDisX, edit->x_at(c_point+1)-minDisX);
    else
        x_val = MAX(edit->x_at(c_point-1)+minDisX, X_VAL);
    edit->set(c_point, x_val, check_data(Y_VAL, 0, 1));
    emit update_myGraph();
    emit update_pointText();
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
    else if (result == QMessageBox::Discard)
        ;
    else if (result == QMessageBox::Cancel)
        event->ignore();
    return;
}

void WindowWaveDesigner::on_pushButtonNew_clicked()
{
    int i = witch_pointClicked();
    if (i == -1 || i == edit->count()-1)
        edit->add(edit->x_at(edit->count()-1)+disX, edit->y_at(edit->count()-1));
    else if (edit->x_at(i+1)-edit->x_at(i) > 10*minDisX)
        edit->insert(i+1, (edit->x_at(i+1)+edit->x_at(i))/2.0, edit->y_at(i));
    emit save_step();
    emit update_myGraph();
}

void WindowWaveDesigner::on_pushButtonSave_clicked()
{
    emit send_waveData(edit);
}

void WindowWaveDesigner::on_pushButtonDelete_clicked()
{
    if (witch_pointClicked() == 0)
        QMessageBox::warning(this, "提示", "暂不支持删除起始点", QMessageBox::Ok, QMessageBox::Ok); //不能删除初始点，或者删除后重排列
    else if (witch_pointClicked() != -1)
    {
        edit->del(witch_pointClicked());
        emit save_step();
        c_point = -1;
        emit choose_point(-1);
        emit update_pointText();
    }
    emit update_myGraph();
}

void WindowWaveDesigner::on_lineEditPointNumber_textEdited(const QString &arg1)
{
    if (arg1 != "")
    {
        if (arg1.toInt() < edit->count() && arg1.toInt() >= 0)
        {
            emit choose_point(arg1.toInt());
            emit update_pointText();
        }
        else
            emit choose_point(-1);
    }
    else
    {
        emit choose_point(-1);
        emit update_pointText();
    }
}

void WindowWaveDesigner::on_lineEditPointTime_textEdited(const QString &arg1)
{
    int i = witch_pointClicked();
    if (i == -1 || arg1 == "" || i == 0)
        return;
    if (i == edit->count()-1)
        edit->set_x(i, MAX(edit->x_at(i-1), arg1.toDouble()));
    else
        edit->set_x(i, check_data(arg1.toDouble(), edit->x_at(i-1)+minDisX, edit->x_at(i+1)-minDisX));
    emit update_myGraph();
}

void WindowWaveDesigner::on_lineEditPointVoltage_textEdited(const QString &arg1)
{
    int i = witch_pointClicked();
    if (i == -1 || arg1 == "")
        return;
    edit->set_y(i, check_data(arg1.toDouble()/100, 0, 1));
    emit update_myGraph();
}

void WindowWaveDesigner::on_lineEditPointNumber_editingFinished()
{
    if (ui->lineEditPointNumber->text().toInt() >= edit->count()) //序号部分
    {
        emit choose_point(-1);
        emit update_pointText();
        QMessageBox::warning(this, "注意", "点序号超出范围", QMessageBox::Ok, QMessageBox::Ok);
        return;
    }
    emit save_step();
}

void WindowWaveDesigner::on_lineEditPointTime_editingFinished()
{
    int i = ui->lineEditPointNumber->text().toInt();
    if (i == 0) //时间部分
    {
        if (ui->lineEditPointTime->text().toDouble() != 0)
        {
            emit drop_step();
            QMessageBox::warning(this, "注意", "暂不能更改起始点的时间", QMessageBox::Ok, QMessageBox::Ok);
            return;
        }
    }
    else
    {
        if (ui->lineEditPointTime->text().toDouble() < edit->x_at(i-1)+minDisX)
        {
            emit drop_step();
            QMessageBox::warning(this, "注意", "点时间过小", QMessageBox::Ok, QMessageBox::Ok);
            return;
        }
        if (i < edit->count()-1 && ui->lineEditPointTime->text().toDouble() > edit->x_at(i+1)-minDisX)
        {
            emit drop_step();
            QMessageBox::warning(this, "注意", "点时间过大", QMessageBox::Ok, QMessageBox::Ok);
            return;
        }
    }
    emit save_step();
}

void WindowWaveDesigner::on_lineEditPointVoltage_editingFinished()
{
    if (ui->lineEditPointVoltage->text().toDouble() > 100)
    {
        emit drop_step();
        QMessageBox::warning(this, "注意", "点电压超出范围", QMessageBox::Ok, QMessageBox::Ok);
        return;
    }
    emit save_step();
}
