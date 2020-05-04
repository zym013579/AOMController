#include "dialogparameter.h"
#include "ui_dialogparameter.h"

#include <QRegExpValidator>

#include "command.h"
#include "init.h"

DialogParameter::DialogParameter(QWidget *parent) :
    QDialog(parent),
    realTimeQuantify(DEFAULT_REALTIME_QUANTIFY),
    canceled(true),
    VolQuantiLevel(DEFAULT_VOL_QUANTIFY_LEVEL),
    maxVol(1),
    unitTime(DEFAULT_UNIT_TIME),
    ui(new Ui::DialogParameter)
{
    ui->setupUi(this);
    setWindowFlags(Qt::WindowCloseButtonHint);

    QRegExp regExpUDouble("(\\d*)(\\.\\d+)?");
    QRegExp regExpUInt("(\\d*)");
    ui->lineEditMaxVol->setValidator(new QRegExpValidator(regExpUDouble, ui->lineEditMaxVol));
    ui->lineEditUnitTime->setValidator(new QRegExpValidator(regExpUDouble, ui->lineEditUnitTime));
    ui->lineEditVolQuantiLevel->setValidator(new QRegExpValidator(regExpUInt, ui->lineEditVolQuantiLevel));

    ui->checkBoxRealTimeQuanti->setChecked(DEFAULT_REALTIME_QUANTIFY);
    ui->lineEditUnitTime->setText(numberToStr(DEFAULT_UNIT_TIME));
    ui->lineEditVolQuantiLevel->setText(numberToStr(DEFAULT_VOL_QUANTIFY_LEVEL));
    ui->lineEditMaxVol->setText(numberToStr(maxVol));
}

DialogParameter::~DialogParameter()
{
    delete ui;
}

void DialogParameter::on_buttonBox_accepted()
{
    realTimeQuantify = ui->checkBoxRealTimeQuanti->isChecked();
    unitTime = ui->lineEditUnitTime->text().toDouble();
    VolQuantiLevel = ui->lineEditVolQuantiLevel->text().toInt();
    maxVol = ui->lineEditMaxVol->text().toDouble();
    canceled = false;
}

void DialogParameter::accept()
{
    if (ui->lineEditUnitTime->text().isEmpty() || ui->lineEditMaxVol->text().isEmpty() || ui->lineEditVolQuantiLevel->text().isEmpty())
    {
        MSG_WARNING("部分参数未设置");
        return;
    }
    return QDialog::accept();
}
