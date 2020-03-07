#include "windowwavedesigner.h"
#include "ui_windowwavedesigner.h"

WindowWaveDesigner::WindowWaveDesigner(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WindowWaveDesigner)
{
    ui->setupUi(this);
}

WindowWaveDesigner::~WindowWaveDesigner()
{
    delete ui;
}
