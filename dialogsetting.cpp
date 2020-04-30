#include "dialogsetting.h"
#include "ui_dialogsetting.h"

DialogSetting::DialogSetting(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSetting)
{
    ui->setupUi(this);
    QRegExp regExpUDouble("(\\d*)(\\.\\d+)?");
    QRegExp regExpUInt("(\\d*)");

    ui->lineEditMinDeltaTime->setValidator(new QRegExpValidator(regExpUDouble, ui->lineEditMinDeltaTime));
    ui->lineEditVolQuantiLevel->setValidator(new QRegExpValidator(regExpUInt, ui->lineEditVolQuantiLevel));

    connect(parent, SIGNAL(sendSettings(bool, int, double)), this, SLOT(recieveSettings(bool, int, double)));
}

DialogSetting::~DialogSetting()
{
    delete ui;
}

void DialogSetting::recieveSettings(bool rTQ, int vQL, double mDT)
{
    ui->lineEditMinDeltaTime->setText(numberToStr(mDT));
    ui->lineEditVolQuantiLevel->setText(numberToStr(vQL+1));
    ui->checkBoxRealTimeQuanti->setEnabled(rTQ);
}

void DialogSetting::on_buttonBox_accepted()
{
    sendSettings(ui->checkBoxRealTimeQuanti->isChecked(), ui->lineEditVolQuantiLevel->text().toUInt()-1, ui->lineEditMinDeltaTime->text().toDouble());
}
