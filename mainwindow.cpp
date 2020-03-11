#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButtonEditWave_clicked()
{
    WindowWaveDesigner *wDesigner = new WindowWaveDesigner;
    wDesigner->show();
    return;
}

void MainWindow::on_actionAbout_triggered()
{
    DialogAbout *wAbout = new DialogAbout;
    wAbout->exec();
    return;
}
