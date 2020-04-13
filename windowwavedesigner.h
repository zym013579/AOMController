#ifndef WINDOWWAVEDESIGNER_H
#define WINDOWWAVEDESIGNER_H

#define MAX(a,b)  (((a)>(b))?(a):(b))
#define MIN(a,b)  (((a)<(b))?(a):(b))

#define X_VAL (ui->widgetWave->xAxis->pixelToCoord(event->pos().x()))
#define Y_VAL (ui->widgetWave->yAxis->pixelToCoord(event->pos().y()))

#define X_RANGE ((ui->widgetWave->width()-34-15)/30.0*5/ui->widgetWave->xAxis->range().size()*11/point_circleSize)
#define Y_RANGE ((ui->widgetWave->height()-14-24)/6.2/ui->widgetWave->yAxis->range().size()*11/point_circleSize)

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
    void recieve_waveData(WaveData *data);

public:
    explicit WindowWaveDesigner(QWidget *parent = nullptr);
    ~WindowWaveDesigner();

private slots:
    void update_myGraph();

    void update_pointText();

    void check_pointText();

    void update_pointNumber();

    void update_pointData();

    void save_step();

    void drop_step();

    void undo_step();

    void redo_step();

    void fresh_undo_redo_button();

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
double check_data(double input, double min, double max);

#endif // WINDOWWAVEDESIGNER_H
