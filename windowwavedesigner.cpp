#include "windowwavedesigner.h"
#include "ui_windowwavedesigner.h"

//QVector<double> WaveData::EditX, WaveData::EditY, WaveData::ModuX, WaveData::ModuY;
bool notmove;

WindowWaveDesigner::WindowWaveDesigner(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WindowWaveDesigner),
    edit(new WaveData),
    mouseHasMoved(false),
    c_point(-1)
{
    ui->setupUi(this);
    init_waveGraph(ui->widgetWave);
    //ui->widgetWave->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(Qt::black, 1.5), QBrush(Qt::white), 9));
    ui->widgetWave->graph()->setLineStyle(QCPGraph::LineStyle::lsStepLeft);
    ui->widgetWave->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ScatterShape::ssCircle, 11));
    //ui->widgetWave->graph()->setSelectable(QCP::iSelectPlottables);
    //ui->widgetWave->item(1)->setSelected(0);
    ui->widgetWave->setInteractions(QCP::iRangeDrag|QCP::iRangeZoom);// QCP::iSelectPlottables  QCP::iRangeZoom
    update_myGraph();
    ui->widgetWave->graph()->setSelectable(QCP::SelectionType::stSingleData);
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

void WindowWaveDesigner::refresh_selectPoint()
{
    //QCPDataRange x = QCPDataRange(2, 3);
    //x->setBegin(2);
    //x->setEnd(3);
    //selected = ui->widgetWave->graph(0)->selection().addDataRange(QCPDataSelection)
}

void WindowWaveDesigner::update_myGraph()
{
    update_waveGraph(ui->widgetWave, edit->x(), edit->y());
}

void WindowWaveDesigner::choose_point(int i)
{
    ui->widgetWave->graph()->setSelection(QCPDataSelection(QCPDataRange(i, i+1)));
    update_myGraph();
}

void WindowWaveDesigner::widgetMousePress(QMouseEvent *event)
{
    ui->widgetWave->graph()->setSelection(QCPDataSelection());
    update_myGraph();
    mouseHasMoved = false;
    c_point = -1;

    double x_val = ui->widgetWave->xAxis->pixelToCoord(event->pos().x());
    double y_val = ui->widgetWave->yAxis->pixelToCoord(event->pos().y());

    double x_range = ui->widgetWave->xAxis->range().size();
    double y_range = ui->widgetWave->yAxis->range().size();

    qDebug() << x_range;
    qDebug() << y_range;

    double tmp = 15;

    for (int i=0; i<edit->count(); i++)
    {
        if (x_val-edit->x_at(i) > x_range/tmp)
            continue;
        else if (qPow(x_val-edit->x_at(i), 2)/x_range+qPow(y_val-edit->y_at(i), 2)/y_range < 1/qPow(tmp, 2))
        {
            c_point = i;
            choose_point(i);
            break;
        }
        else
            break;
    }
    //ui->widgetWave->graph(0)->selectionChanged()
}

void WindowWaveDesigner::widgetMouseRelease(QMouseEvent *event)
{
    ;
    qDebug() << ui->widgetWave->xAxis->range();

}

void WindowWaveDesigner::widgetMouseMove(QMouseEvent *event)
{
    ;
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

    QCPDataRange x = QCPDataRange(2, 3);
    QCPDataSelection y = QCPDataSelection(x);
    //x->setBegin(2);
    //x->setEnd(3);
    ui->widgetWave->graph(0)->setSelection(y);
}

void WindowWaveDesigner::on_pushButtonSave_clicked()
{
    edit->clear();
    for (int i = 0; i < 10; i++)
    {
        edit->add(i/1.0, rand()%2);
    }
    update_myGraph();
}
