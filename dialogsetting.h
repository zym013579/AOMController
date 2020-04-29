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

    bool realTimeQuantify;

    int volQuantiLevel;

    double minDeltaTime;

signals:
    void sendSettings(bool rTQ, int vQL, double mDT);
};

#endif // DIALOGSETTING_H
