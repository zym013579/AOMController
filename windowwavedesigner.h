#ifndef WINDOWWAVEDESIGNER_H
#define WINDOWWAVEDESIGNER_H

#define MAX(a,b)  (((a)>(b))?(a):(b))
#define MIN(a,b)  (((a)<(b))?(a):(b))

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

public:
    explicit WindowWaveDesigner(QWidget *parent = nullptr);
    ~WindowWaveDesigner();

private slots:
    void update_myGraph();

    int if_pointClicked(QMouseEvent *event);

    int witch_pointClicked();

    void choose_point(int i);

    void on_pushButtonNew_clicked();

    void widgetMousePress(QMouseEvent *event);

    void widgetMouseRelease(QMouseEvent *event);

    void widgetMouseMove(QMouseEvent *event);

    void on_pushButtonSave_clicked();

    void on_pushButtonDelete_clicked();

private:
    Ui::WindowWaveDesigner *ui;

    WaveData *edit;

    bool mouseHasMoved;

    int c_point, point_circleSize;

    double disX, minDisX;

    void closeEvent(QCloseEvent *event);
signals:
    void send_waveData(WaveData *data);
};

void init_waveGraph(QCustomPlot *target);
void update_waveGraph(QCustomPlot *target, QVector<double> x, QVector<double> y);

#endif // WINDOWWAVEDESIGNER_H
