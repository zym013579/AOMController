#include "dialogabout.h"
#include "ui_dialogabout.h"

DialogAbout::DialogAbout(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogAbout)
{
    ui->setupUi(this);
    setWindowFlags(Qt::WindowCloseButtonHint);
}

DialogAbout::~DialogAbout()
{
    delete ui;
}
