#ifndef WINDOWWAVEDESIGNER_H
#define WINDOWWAVEDESIGNER_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QMessageBox>

namespace Ui {
class WindowWaveDesigner;
}

class WindowWaveDesigner : public QMainWindow
{
    Q_OBJECT

public:
    explicit WindowWaveDesigner(QWidget *parent = nullptr);
    ~WindowWaveDesigner();

private:
    Ui::WindowWaveDesigner *ui;

    void closeEvent(QCloseEvent *event);
};

#endif // WINDOWWAVEDESIGNER_H
