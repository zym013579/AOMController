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
    //ui->widgetWave->graph()->setLineStyle(QCPGraph::LineStyle::lsStepLeft); //方波图形
    ui->widgetWave->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ScatterShape::ssCircle, point_circleSize));
    ui->widgetWave->setInteractions(QCP::iRangeDrag|QCP::iRangeZoom);// QCP::iSelectPlottables  QCP::iRangeZoom
    ui->widgetWave->graph()->setSelectable(QCP::SelectionType::stSingleData);   //仅可选择一个点
    ui->widgetWave->axisRect()->setRangeDrag(Qt::Horizontal);   //设置仅水平拖动和缩放
    ui->widgetWave->axisRect()->setRangeZoom(Qt::Horizontal);

    emit edit->init();
    emit update_myGraph();

    connect(ui->widgetWave, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(widgetMousePress(QMouseEvent*)));
    connect(ui->widgetWave, SIGNAL(mouseRelease(QMouseEvent*)), this, SLOT(widgetMouseRelease(QMouseEvent*)));
    connect(ui->widgetWave, SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(widgetMouseMove(QMouseEvent*)));

    connect(ui->lineEditPointNumber, SIGNAL(textEdited(QString)), this, SLOT(update_pointNumber()));
    connect(ui->lineEditPointTime, SIGNAL(textEdited(QString)), this, SLOT(update_pointData()));
    connect(ui->lineEditPointVoltage, SIGNAL(textEdited(QString)), this, SLOT(update_pointData()));
    connect(ui->lineEditPointNumber, SIGNAL(editingFinished()), this, SLOT(check_pointText()));
    connect(ui->lineEditPointTime, SIGNAL(editingFinished()), this, SLOT(check_pointText()));
    connect(ui->lineEditPointVoltage, SIGNAL(editingFinished()), this, SLOT(check_pointText()));
}

WindowWaveDesigner::~WindowWaveDesigner()
{
    edit->clear();
    delete edit;
    delete ui;
}

/*比较input与最大最小值，返回适当的值*/
double check_data(double input, double min, double max)
{
    return MAX(min, MIN(max, input));
}

void WindowWaveDesigner::update_myGraph()
{
    emit update_waveGraph(ui->widgetWave, edit->x().toVector(), edit->y().toVector());
    //emit update_pointText();
}

/*根据选点更新文本框*/
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

void WindowWaveDesigner::check_pointText()
{
    int i = ui->lineEditPointNumber->text().toInt();
    if (i >= edit->count()) //序号部分
    {
        choose_point(-1);
        QMessageBox::warning(this, "注意", "点序号超出范围", QMessageBox::Ok, QMessageBox::Ok);
        return;
    }

    if (i == 0) //时间部分
    {
        if (ui->lineEditPointTime->text().toDouble() != 0)
        {
            edit->unDo();
            edit->save();
            update_myGraph();
            choose_point(i);
            QMessageBox::warning(this, "注意", "暂不能更改起始点的时间", QMessageBox::Ok, QMessageBox::Ok);
            return;
        }
    }
    else
    {
        if (ui->lineEditPointTime->text().toDouble() < edit->x_at(i-1)+minDisX)
        {
            edit->unDo();
            edit->save();
            update_myGraph();
            choose_point(i);
            QMessageBox::warning(this, "注意", "点时间过小", QMessageBox::Ok, QMessageBox::Ok);
            return;
        }
        if (i < edit->count()-1 && ui->lineEditPointTime->text().toDouble() > edit->x_at(i+1)-minDisX)
        {
            edit->unDo();
            edit->save();
            update_myGraph();
            choose_point(i);
            QMessageBox::warning(this, "注意", "点时间过大", QMessageBox::Ok, QMessageBox::Ok);
            return;
        }
    }

    if (ui->lineEditPointVoltage->text().toDouble() > 100)
    {
        edit->unDo();
        edit->save();
        update_myGraph();
        choose_point(i);
        QMessageBox::warning(this, "注意", "点电压超出范围", QMessageBox::Ok, QMessageBox::Ok);
        return;
    }
    edit->save();
}

/*根据文本框更新选点*/
void WindowWaveDesigner::update_pointNumber()
{
    int i = ui->lineEditPointNumber->text().toInt();
    if (ui->lineEditPointNumber->text() != "")
    {
        if (i < edit->count() && i >= 0)
            emit choose_point(i);
        else
            emit choose_point(-2);
    }
    else
        emit choose_point(-1);
}

/*用文本框数据更新图形（不排错）*/
void WindowWaveDesigner::update_pointData()
{
    if (witch_pointClicked() == -1)
        return;
    int i = ui->lineEditPointNumber->text().toInt();
    if (i == 0)
        edit->set(0, 0, check_data(ui->lineEditPointVoltage->text().toDouble()/100, 0, 1), false);
    else if (i == edit->count()-1)
        edit->set(i, MAX(edit->x_at(i-1), ui->lineEditPointTime->text().toDouble()), check_data(ui->lineEditPointVoltage->text().toDouble()/100, 0, 1), false);
    else
        edit->set(i, check_data(ui->lineEditPointTime->text().toDouble(), edit->x_at(i-1)+minDisX, edit->x_at(i+1)-minDisX), check_data(ui->lineEditPointVoltage->text().toDouble()/100, 0, 1), false);
    emit update_myGraph();
}

/*是否正在点击某个点，若是返回序号*/
int WindowWaveDesigner::if_pointClicked(QMouseEvent *event)
{
    //double x_val = ui->widgetWave->xAxis->pixelToCoord(event->pos().x());
    //double y_val = ui->widgetWave->yAxis->pixelToCoord(event->pos().y());

    //以下两行式子中的常数均为实际测得
    //double x_range = (ui->widgetWave->width()-34-15)/30.0*5/ui->widgetWave->xAxis->range().size()*11/point_circleSize;
    //double y_range = (ui->widgetWave->height()-14-24)/6.2/ui->widgetWave->yAxis->range().size()*11/point_circleSize;
    /* debug
    qDebug() << "x_pos:" << event->pos().x();
    qDebug() << "y_pos:" << event->pos().y();
    qDebug() << "x_range:" << x_range;
    qDebug() << "y_range:" << y_range;
    qDebug() << "x_val:" << x_val;
    qDebug() << "y_val:" << y_val;
    */
    //double tmp = 15;

    for (int i=0; i<edit->count(); i++)
    {
        /* debug
        qDebug() << "i:" << i;
        qDebug() << "count:" << edit->count();
        qDebug() << "x_at:" << edit->x_at(i);
        qDebug() << "y_at:" << edit->y_at(i);
        qDebug() << "xout:" << qPow((x_val-edit->x_at(i))*x_range, 2);
        qDebug() << "yout:" << qPow((y_val-edit->y_at(i))*y_range, 2);
        qDebug() << "\n";
        */
        if ((X_VAL-edit->x_at(i))*X_RANGE > 1)
            continue;
        else if (qPow((X_VAL-edit->x_at(i))*X_RANGE, 2)+qPow((Y_VAL-edit->y_at(i))*Y_RANGE, 2) <= 1)
            return i;
        else if ((X_VAL-edit->x_at(i))*X_RANGE < -1)
            break;
    }
    return -1;
}

/*是否已经选中了某个点，若是返回序号*/
int WindowWaveDesigner::witch_pointClicked()
{
    if (ui->widgetWave->graph()->selection() == QCPDataSelection())
        return -1;
    return ui->widgetWave->graph()->selection().dataRange().begin();
}

/*选中某个点，其中-1为取消选点并更新文本框，-2则不更新文本框*/
void WindowWaveDesigner::choose_point(int i)
{
    if (i > -1)
        ui->widgetWave->graph()->setSelection(QCPDataSelection(QCPDataRange(i, i+1)));
    else
        ui->widgetWave->graph()->setSelection(QCPDataSelection());
    if (i != -2)
        emit update_pointText();
    emit update_myGraph();
}

void WindowWaveDesigner::widgetMousePress(QMouseEvent *event)
{
    mouseHasMoved = false;
    c_point = -1;
    if (if_pointClicked(event) == witch_pointClicked())
        c_point = witch_pointClicked();
}

void WindowWaveDesigner::widgetMouseRelease(QMouseEvent *event)
{
    if (!mouseHasMoved)
        emit choose_point(if_pointClicked(event));
    else if (c_point != -1)
        edit->save();
    c_point = -1;
    ui->widgetWave->setInteractions(QCP::iRangeDrag|QCP::iRangeZoom);
}

void WindowWaveDesigner::widgetMouseMove(QMouseEvent *event)
{
    mouseHasMoved = true;
    if (c_point == -1)
        return;
    ui->widgetWave->setInteractions(QCP::iRangeZoom);
    //double x_val = ui->widgetWave->xAxis->pixelToCoord(event->pos().x());
    //double y_val = ui->widgetWave->yAxis->pixelToCoord(event->pos().y());

    double x_val;
    if (c_point == 0)
        x_val = 0;
    else if (c_point != edit->count()-1)
        x_val = check_data(X_VAL, edit->x_at(c_point-1)+minDisX, edit->x_at(c_point+1)-minDisX);
    else
        x_val = MAX(edit->x_at(c_point-1)+minDisX, X_VAL);
    edit->set(c_point, x_val, check_data(Y_VAL, 0, 1), false);
    emit update_myGraph();
    emit update_pointText();
}

void WindowWaveDesigner::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton result = QMessageBox::information(this, "提示", "波形尚未应用，是否应用波形？", QMessageBox::Yes|QMessageBox::Save|QMessageBox::Discard|QMessageBox::Cancel, QMessageBox::Cancel);
    //event->accept();
    //event->ignore();
    if (result == QMessageBox::Yes)
        ;
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
    //qDebug() << ui->widgetWave->itemCount();
    //ui->widgetWave->graph()->selectTest();

    //ui->widgetWave->graph()->layer()->children();

    //QCPDataRange x = QCPDataRange(2, 3);
    //QCPDataSelection y = QCPDataSelection(x);
    //x->setBegin(2);
    //x->setEnd(3);
    //ui->widgetWave->graph(0)->setSelection(y);
    int i = witch_pointClicked();
    if (i == -1 || i == edit->count()-1)
        edit->add(edit->x_at(edit->count()-1)+disX, edit->y_at(edit->count()-1));
    else if (edit->x_at(i+1)-edit->x_at(i) > 10*minDisX)
        edit->insert(i+1, (edit->x_at(i+1)+edit->x_at(i))/2.0, edit->y_at(i));
    emit update_myGraph();
}

void WindowWaveDesigner::on_pushButtonSave_clicked()
{
    //* test
    //qDebug() << ui->lineEditPointNumber->text().toInt() << "\n" << ui->lineEditPointVoltage->text().toDouble() << "\n\n";
    //*/
    //emit send_waveData(edit);
    edit->unDo();
    update_myGraph();
    update_pointText();
}

void WindowWaveDesigner::on_pushButtonDelete_clicked()
{
    if (witch_pointClicked() == 0)
        ; //不能删除初始点，或者删除后重排列
    else if (witch_pointClicked() != -1)
    {
        edit->del(witch_pointClicked());
        c_point = -1;
        emit choose_point(-1);
    }
    emit update_myGraph();
}
