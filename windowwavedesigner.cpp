#include "windowwavedesigner.h"
#include "ui_windowwavedesigner.h"

//QVector<double> WaveData::EditX, WaveData::EditY, WaveData::ModuX, WaveData::ModuY;
//bool notmove;

WindowWaveDesigner::WindowWaveDesigner(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WindowWaveDesigner),
    edit(new WaveData),
    mouseHasMoved(false),
    c_point(-1),
    point_circleSize(13),    //图形中的点的大小
    disX(0.5),  //新建点默认间距
    minDisX(0.01)   //允许最短间距
{
    ui->setupUi(this);
    emit init_waveGraph(ui->widgetWave);
    //ui->widgetWave->graph()->setLineStyle(QCPGraph::LineStyle::lsStepLeft); //方波图形
    ui->widgetWave->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ScatterShape::ssCircle, point_circleSize));
    ui->widgetWave->setInteractions(QCP::iRangeDrag|QCP::iRangeZoom);// QCP::iSelectPlottables  QCP::iRangeZoom
    ui->widgetWave->graph()->setSelectable(QCP::SelectionType::stSingleData);   //仅可选择一个点

    edit->add(0, 0);
    emit update_myGraph();

    connect(ui->widgetWave,SIGNAL(mousePress(QMouseEvent*)),this,SLOT(widgetMousePress(QMouseEvent*)));
    connect(ui->widgetWave,SIGNAL(mouseRelease(QMouseEvent*)),this,SLOT(widgetMouseRelease(QMouseEvent*)));
    connect(ui->widgetWave,SIGNAL(mouseMove(QMouseEvent*)),this,SLOT(widgetMouseMove(QMouseEvent*)));
}

WindowWaveDesigner::~WindowWaveDesigner()
{
    edit->clear();
    delete edit;
    delete ui;
}

/*
void WindowWaveDesigner::refresh_selectPoint()
{
    //QCPDataRange x = QCPDataRange(2, 3);
    //x->setBegin(2);
    //x->setEnd(3);
    //selected = ui->widgetWave->graph(0)->selection().addDataRange(QCPDataSelection)
}
*/

void WindowWaveDesigner::update_myGraph()
{
    emit update_waveGraph(ui->widgetWave, edit->x(), edit->y());
}

/*是否正在点击某个点，若是返回序号*/
int WindowWaveDesigner::if_pointClicked(QMouseEvent *event)
{
    double x_val = ui->widgetWave->xAxis->pixelToCoord(event->pos().x());
    double y_val = ui->widgetWave->yAxis->pixelToCoord(event->pos().y());

    //以下两行式子中的常数均为实际测得
    double x_range = (ui->widgetWave->width()-34-15)/30.0*5/ui->widgetWave->xAxis->range().size()*11/point_circleSize;
    double y_range = (ui->widgetWave->height()-14-24)/6.2/ui->widgetWave->yAxis->range().size()*11/point_circleSize;
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
        if ((x_val-edit->x_at(i))*x_range > 1)
            continue;
        else if (qPow((x_val-edit->x_at(i))*x_range, 2)+qPow((y_val-edit->y_at(i))*y_range, 2) <= 1)
            return i;
        else if ((x_val-edit->x_at(i))*x_range < -1)
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

/*选中某个点*/
void WindowWaveDesigner::choose_point(int i)
{
    if (i != -1)
        ui->widgetWave->graph()->setSelection(QCPDataSelection(QCPDataRange(i, i+1)));
    else
        ui->widgetWave->graph()->setSelection(QCPDataSelection());
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
    {
        emit choose_point(if_pointClicked(event));
        emit update_myGraph();
    }
    c_point = -1;
    ui->widgetWave->setInteractions(QCP::iRangeDrag|QCP::iRangeZoom);
}

void WindowWaveDesigner::widgetMouseMove(QMouseEvent *event)
{
    mouseHasMoved = true;
    if (c_point == -1)
        return;
    ui->widgetWave->setInteractions(QCP::iRangeZoom);
    double x_val = ui->widgetWave->xAxis->pixelToCoord(event->pos().x());
    double y_val = ui->widgetWave->yAxis->pixelToCoord(event->pos().y());

    if (c_point == 0)
        x_val = 0;
    else if (c_point != edit->count()-1)
    {
        x_val = MAX(edit->x_at(c_point-1)+minDisX, x_val);
        x_val = MIN(edit->x_at(c_point+1)-minDisX, x_val);
    }
    else
        x_val = MAX(edit->x_at(c_point-1)+minDisX, x_val);
    y_val = MIN(1, y_val);
    y_val = MAX(0, y_val);
    edit->set(c_point, x_val, y_val);
    emit update_myGraph();
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
    if (i == -1)
        edit->add(edit->x_at(edit->count()-1)+disX, edit->y_at(edit->count()-1));
    else if (edit->x_at(i+1)-edit->x_at(i) > 10*minDisX)
        edit->insert(i+1, (edit->x_at(i+1)+edit->x_at(i))/2.0, edit->y_at(i));
    emit update_myGraph();
}

void WindowWaveDesigner::on_pushButtonSave_clicked()
{
    /*
    edit->clear();
    for (int i = 0; i < 10; i++)
        edit->add(i/1.0, rand()%2);
    emit update_myGraph();
    */
    send_waveData(edit);
}

void WindowWaveDesigner::on_pushButtonDelete_clicked()
{
    if (witch_pointClicked() == 0)
        ; //不能删除初始点，或者删除后重排列
    else if (witch_pointClicked() != -1)
    {
        edit->del(witch_pointClicked());
        c_point = -1;
        choose_point(-1);
    }
    emit update_myGraph();
}
