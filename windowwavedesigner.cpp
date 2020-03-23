#include "windowwavedesigner.h"
#include "ui_windowwavedesigner.h"

//QVector<double> WaveData::EditX, WaveData::EditY, WaveData::ModuX, WaveData::ModuY;

WindowWaveDesigner::WindowWaveDesigner(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WindowWaveDesigner)
{
    ui->setupUi(this);
    init_waveGraph(ui->widgetWave);
    //ui->widgetWave->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(Qt::black, 1.5), QBrush(Qt::white), 9));
    ui->widgetWave->setInteractions(QCP::iRangeDrag|QCP::iRangeZoom);
    update_waveGraph(ui->widgetWave, WaveData::EditX, WaveData::EditY);
    connect(ui->widgetWave,SIGNAL(mousePress(QMouseEvent*)),this,SLOT(widgetMousePress(QMouseEvent*)));
    connect(ui->widgetWave,SIGNAL(mouseRelease(QMouseEvent*)),this,SLOT(widgetMouseRelease(QMouseEvent*)));
    connect(ui->widgetWave,SIGNAL(mouseMove(QMouseEvent*)),this,SLOT(widgetMouseMove(QMouseEvent*)));
}

WindowWaveDesigner::~WindowWaveDesigner()
{
    delete ui;
}

void WindowWaveDesigner::widgetMousePress(QMouseEvent *event)
{
    ;
}

void WindowWaveDesigner::widgetMouseRelease(QMouseEvent *event)
{
    ;
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
    {
        update_waveGraph(ui->widgetWave, WaveData::ModuX, WaveData::ModuY);
        event->ignore();
    }
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
    ;
}
