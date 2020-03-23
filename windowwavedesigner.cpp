#include "windowwavedesigner.h"
#include "ui_windowwavedesigner.h"

WindowWaveDesigner::WindowWaveDesigner(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WindowWaveDesigner)
{
    ui->setupUi(this);
    init_waveGraph(ui->widgetWave);
}

WindowWaveDesigner::~WindowWaveDesigner()
{
    delete ui;
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
