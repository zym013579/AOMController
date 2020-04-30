#include "wavedata.h"

WaveData::WaveData() :
    realTimeQuantify(DEFAULT_REALTIME_QUANTIFY),
    volQuantiLevel(DEFAULT_VOL_QUANTIFY_LEVEL),
    minDeltaTime(DEFAULT_MIN_DELTA_TIME),
    dataX(*(new QList<double>)),
    dataY(*(new QList<double>)),
    historyX(*(new QList<QList<double>>)),
    historyY(*(new QList<QList<double>>)),
    futureX(*(new QList<QList<double>>)),
    futureY(*(new QList<QList<double>>)),
    historyVQL(*(new QList<int>)),
    futureVQL(*(new QList<int>)),
    historyMDT(*(new QList<double>)),
    futureMDT(*(new QList<double>))
{
    clear();
    dataX.append(0);
    dataY.append(0);
    save();
}

WaveData::~WaveData()
{
    clear();
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
    if (realTimeQuantify)
    {
        if (minDeltaTime > 0)
            x = round(x/minDeltaTime)*minDeltaTime;
        if (volQuantiLevel > 0)
            y = round(y*volQuantiLevel)/volQuantiLevel;
    }
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
    historyMDT.clear();
    historyVQL.clear();
    futureMDT.clear();
    futureVQL.clear();
}

void WaveData::copyData(WaveData *data)
{
    dataX.clear();
    dataY.clear();
    dataX.append(data->x());
    dataY.append(data->y());
    realTimeQuantify = data->getRealTimeQuantify();
    volQuantiLevel = data->getVolQuantiLevel();
    minDeltaTime = data->getMinDeltaTime();
    save();
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
    if (realTimeQuantify)
    {
        if (minDeltaTime > 0)
            x = round(x/minDeltaTime)*minDeltaTime;
        if (volQuantiLevel > 0)
            y = round(y*volQuantiLevel)/volQuantiLevel;
    }
    set_x(i, x);
    set_y(i, y);
}

void WaveData::set_x(int i, double x)
{
    if (realTimeQuantify)
    {
        if (minDeltaTime > 0)
            x = round(x/minDeltaTime)*minDeltaTime;
    }
    dataX.replace(i, x);
}

void WaveData::set_y(int i, double y)
{
    if (realTimeQuantify)
    {
        if (volQuantiLevel > 0)
            y = round(y*volQuantiLevel)/volQuantiLevel;
    }
    dataY.replace(i, y);
}

void WaveData::del(int i)
{
    dataX.removeAt(i);
    dataY.removeAt(i);
}

void WaveData::insert(int i, double x, double y)
{
    if (realTimeQuantify)
    {
        if (minDeltaTime > 0)
            x = round(x/minDeltaTime)*minDeltaTime;
        if (volQuantiLevel > 0)
            y = round(y*volQuantiLevel)/volQuantiLevel;
    }
    dataX.insert(i, x);
    dataY.insert(i, y);
}

void WaveData::save()
{
    if (historyX.count() > 0 && dataX == historyX.last() && dataY == historyY.last() && minDeltaTime == historyMDT.last() && volQuantiLevel == historyVQL.last())
        return;
    historyX.append(dataX);
    historyY.append(dataY);
    historyMDT.append(minDeltaTime);
    historyVQL.append(volQuantiLevel);
    futureX.clear();
    futureY.clear();
    futureMDT.clear();
    futureVQL.clear();
}

void WaveData::unDo()
{
    futureX.append(dataX);
    futureY.append(dataY);
    futureMDT.append(minDeltaTime);
    futureVQL.append(volQuantiLevel);
    historyX.removeLast();
    historyY.removeLast();
    historyMDT.removeLast();
    historyVQL.removeLast();
    dataX = historyX.last();
    dataY = historyY.last();
    minDeltaTime = historyMDT.last();
    volQuantiLevel = historyVQL.last();
}

int WaveData::count_unDo()
{
    return historyX.count();
}

void WaveData::reDo()
{
    dataX = futureX.last();
    dataY = futureY.last();
    minDeltaTime = futureMDT.last();
    volQuantiLevel = futureVQL.last();
    historyX.append(dataX);
    historyY.append(dataY);
    historyMDT.append(minDeltaTime);
    historyVQL.append(volQuantiLevel);
    futureX.removeLast();
    futureY.removeLast();
    futureMDT.removeLast();
    futureVQL.removeLast();
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

void WaveData::setRealTimeQuantify(bool enabled)
{
    realTimeQuantify = enabled;
    if (realTimeQuantify)
        quantify();
}

bool WaveData::getRealTimeQuantify()
{
    return realTimeQuantify;
}

int WaveData::getVolQuantiLevel()
{
    return volQuantiLevel;
}

double WaveData::getMinDeltaTime()
{
    return minDeltaTime;
}

void WaveData::quantify(double dx, int level)
{
    int i;
    if(dx > 0)
        minDeltaTime = dx;
    if(level > 0)
        volQuantiLevel = level;

    if (!realTimeQuantify)
        return;

    if (minDeltaTime > 0 && dataX.count() > 1)
        for (i = 1; i < dataX.count(); i++)
            set_x(i, round(dataX.at(i)/minDeltaTime)*minDeltaTime);

    if ((1.0/volQuantiLevel) > 0 && dataY.count() > 1)
        for (i = 1; i < dataY.count(); i++)
            set_y(i, round(dataY.at(i)*volQuantiLevel)/volQuantiLevel);
}
