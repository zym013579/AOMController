#include "wavedata.h"

WaveData::WaveData() :
    disX(-1),
    disY(-1),
    dataX(*(new QList<double>)),
    dataY(*(new QList<double>)),
    historyX(*(new QList<QList<double>>)),
    historyY(*(new QList<QList<double>>)),
    futureX(*(new QList<QList<double>>)),
    futureY(*(new QList<QList<double>>))
{
    this->clear();
    dataX.append(0);
    dataY.append(0);
    this->save();
}

WaveData::~WaveData()
{
    this->clear();
}

QList<double> WaveData::x()
{
    return dataX;
}

QList<double> WaveData::y()
{
    return dataY;
}

void WaveData::add(double x, double y)
{
#if (REAL_TIME_QUALITIFY)
    if (disX > 0)
        x = round(x/disX)*disX;
    if (disY > 0)
        y = round(y/disY)*disY;
#endif
    dataX.append(x);
    dataY.append(y);
}

void WaveData::clear()
{
    dataX.clear();
    dataY.clear();
    historyX.clear();
    historyY.clear();
    futureX.clear();
    futureY.clear();
}

void WaveData::copyData(WaveData *data)
{
    dataX.clear();
    dataY.clear();
    dataX.append(data->x());
    dataY.append(data->y());
    this->save();
}

int WaveData::count()
{
    return dataX.size();
}

double WaveData::x_at(int i)
{
    return dataX.at(i);
}

double WaveData::y_at(int i)
{
    return dataY.at(i);
}

void WaveData::set(int i, double x, double y)
{
#if (REAL_TIME_QUALITIFY)
    if (disX > 0)
        x = round(x/disX)*disX;
    if (disY > 0)
        y = round(y/disY)*disY;
#endif
    this->set_x(i, x);
    this->set_y(i, y);
}

void WaveData::set_x(int i, double x)
{
#if (REAL_TIME_QUALITIFY)
    if (disX > 0)
        x = round(x/disX)*disX;
#endif
    dataX.replace(i, x);
}

void WaveData::set_y(int i, double y)
{
#if (REAL_TIME_QUALITIFY)
    if (disY > 0)
        y = round(y/disY)*disY;
#endif
    dataY.replace(i, y);
}

void WaveData::del(int i)
{
    dataX.removeAt(i);
    dataY.removeAt(i);
}

void WaveData::insert(int i, double x, double y)
{
#if (REAL_TIME_QUALITIFY)
    if (disX > 0)
        x = round(x/disX)*disX;
    if (disY > 0)
        y = round(y/disY)*disY;
#endif
    dataX.insert(i, x);
    dataY.insert(i, y);
}

void WaveData::save()
{
    if (historyX.count() > 0 && dataX == historyX.last() && dataY == historyY.last())
        return;
    historyX.append(dataX);
    historyY.append(dataY);
    futureX.clear();
    futureY.clear();
}

void WaveData::unDo()
{
    futureX.append(dataX);
    futureY.append(dataY);
    historyX.removeLast();
    historyY.removeLast();
    dataX = historyX.last();
    dataY = historyY.last();
}

int WaveData::count_unDo()
{
    return historyX.count();
}

void WaveData::reDo()
{
    dataX = futureX.last();
    dataY = futureY.last();
    historyX.append(dataX);
    historyY.append(dataY);
    futureX.removeLast();
    futureY.removeLast();
}

int WaveData::count_reDo()
{
    return futureX.count();
}

void WaveData::drop()
{
    dataX = historyX.last();
    dataY = historyY.last();
}

void WaveData::setDisX(double x)
{
    if(x > 0)
        disX = x;
}

void WaveData::setDisY(double y)
{
    if(y > 0)
        disY = y;
}

void WaveData::qualitify(double x, double y)
{
    int i;
    this->setDisX(x);
    this->setDisY(y);

    if (disX > 0 && dataX.count() > 1)
        for (i = 1; i < dataX.count(); i++)
            this->set_x(i, round(dataX.at(i)/disX)*disX);

    if (disY > 0 && dataY.count() > 1)
        for (i = 1; i < dataY.count(); i++)
            this->set_y(i, round(dataY.at(i)/disY)*disY);
}
