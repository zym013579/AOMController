#ifndef DIALOGSETTING_H
#define DIALOGSETTING_H

#include <QDialog>

namespace Ui {
class DialogSetting;
}

class DialogSetting : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSetting(QWidget *parent = nullptr);
    ~DialogSetting();

private:
    Ui::DialogSetting *ui;

private slots:
    void recieveSettings(bool rTQ, int vQL, double mDT);

    void on_buttonBox_accepted();

signals:
    void sendSettings(bool rTQ, int vQL, double mDT);
};

QString numberToStr(int num);

QString numberToStr(double num);

#endif // DIALOGSETTING_H
