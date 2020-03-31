#ifndef WINDOWWAVEDESIGNER_H
#define WINDOWWAVEDESIGNER_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QMessageBox>
//#include <QtDebug>
#include <QtMath>

#include "qcustomplot.h"
#include "wavedata.h"

namespace Ui {
class WindowWaveDesigner;
}

class WindowWaveDesigner : public QMainWindow
{
    Q_OBJECT

public slots:
    void refresh_selectPoint();

public:
    explicit WindowWaveDesigner(QWidget *parent = nullptr);
    ~WindowWaveDesigner();

private slots:
    void update_myGraph();

    void choose_point(int i);

    void on_pushButtonNew_clicked();

    void widgetMousePress(QMouseEvent *event);

    void widgetMouseRelease(QMouseEvent *event);

    void widgetMouseMove(QMouseEvent *event);

    void on_pushButtonSave_clicked();

private:
    Ui::WindowWaveDesigner *ui;

    WaveData *edit;

    bool mouseHasMoved;

    int c_point;

    void closeEvent(QCloseEvent *event);
};

void init_waveGraph(QCustomPlot *target);
void update_waveGraph(QCustomPlot *target, QVector<double> x, QVector<double> y);

#endif // WINDOWWAVEDESIGNER_H
