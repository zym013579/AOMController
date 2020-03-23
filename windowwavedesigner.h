#ifndef WINDOWWAVEDESIGNER_H
#define WINDOWWAVEDESIGNER_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QMessageBox>

#include "qcustomplot.h"

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

void init_waveGraph(QCustomPlot *target);
void update_waveGraph(QCustomPlot *target, QVector<double> x, QVector<double> y);

#endif // WINDOWWAVEDESIGNER_H
