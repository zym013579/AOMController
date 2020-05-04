#ifndef DIALOGPARAMETER_H
#define DIALOGPARAMETER_H

#include <QDialog>

namespace Ui {
class DialogParameter;
}

class DialogParameter : public QDialog
{
    Q_OBJECT

public:
    explicit DialogParameter(QWidget *parent = nullptr);
    ~DialogParameter();

    bool realTimeQuantify, canceled;

    int VolQuantiLevel;

    double maxVol, unitTime;
private slots:
    void on_buttonBox_accepted();

    void accept();

private:
    Ui::DialogParameter *ui;
};

QString numberToStr(int num);

QString numberToStr(double num);

#endif // DIALOGPARAMETER_H
