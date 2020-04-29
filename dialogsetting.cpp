#include "dialogsetting.h"
#include "ui_dialogsetting.h"

DialogSetting::DialogSetting(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSetting),
    realTimeQuantify(DEFAULT_REALTIME_QUANTIFY),
    volQuantiLevel(DEFAULT_VOL_QUANTIFY_LEVEL),
    minDeltaTime(DEFAULT_MIN_DELTA_TIME)
{
    ui->setupUi(this);

    ui->lineEditMinDeltaTime->setText(QString::number(realTimeQuantify));
    ui->lineEditVolQuantiLevel->setText(QString::number(volQuantiLevel));
    ui->checkBoxRealTimeQuanti->setEnabled(minDeltaTime);
}

DialogSetting::~DialogSetting()
{
    delete ui;
}
