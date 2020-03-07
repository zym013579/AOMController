#ifndef WINDOWWAVEDESIGNER_H
#define WINDOWWAVEDESIGNER_H

#include <QMainWindow>

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
};

#endif // WINDOWWAVEDESIGNER_H
