#include "wavedata.h"

#include "init.h"

WaveData::WaveData() :
    point(-1),
    realTimeQuantify(DEFAULT_REALTIME_QUANTIFY),
    volQuantiLevel(DEFAULT_VOL_QUANTIFY_LEVEL),
    unitTime(DEFAULT_UNIT_TIME),
    maxVol(1),
    dataX(),
    dataY(),
    historyX(),
    historyY(),
    historyVQL(),
    historyMDT(),
    historyMaxVol()
{
    //clear();
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
        if (unitTime > 0)
            x = 1.0*qRound(x/unitTime)*unitTime;
        if (volQuantiLevel > 0)
            y = 1.0*qRound(y*volQuantiLevel)/volQuantiLevel;
    }
    dataX.append(x);
    dataY.append(y);
}

void WaveData::add(QList<double> x, QList<double> y)
{
    dataX.append(x);
    dataY.append(y);
    if (realTimeQuantify)
        quantify();
}

void WaveData::clear()
{
    volQuantiLevel = DEFAULT_VOL_QUANTIFY_LEVEL;
    unitTime = DEFAULT_UNIT_TIME;
    maxVol = 1;
    realTimeQuantify = DEFAULT_REALTIME_QUANTIFY;
    point = -1;
    dataX.clear();
    dataY.clear();
    historyX.clear();
    historyY.clear();
    historyMDT.clear();
    historyVQL.clear();
}

void WaveData::copyData(WaveData *data)
{
    save();
    dataX.clear();
    dataY.clear();
    dataX.append(data->x());
    dataY.append(data->y());
    realTimeQuantify = data->getRealTimeQuantify();
    volQuantiLevel = data->getVolQuantiLevel();
    unitTime = data->getUnitTime();
    maxVol = data->getMaxVol();
    save();
}

int WaveData::count()
{
    return dataX.size();
}

double WaveData::xAt(int i)
{
    return dataX.at(i);
}

double WaveData::yAt(int i)
{
    return dataY.at(i);
}

void WaveData::set(int i, double x, double y)
{
    if (realTimeQuantify)
    {
        if (unitTime > 0)
            x = 1.0*qRound(x/unitTime)*unitTime;
        if (volQuantiLevel > 0)
            y = 1.0*qRound(y*volQuantiLevel)/volQuantiLevel;
    }
    setX(i, x);
    setY(i, y);
}

void WaveData::setX(int i, double x)
{
    if (realTimeQuantify)
    {
        if (unitTime > 0)
            x = 1.0*qRound(x/unitTime)*unitTime;
    }
    dataX.replace(i, x);
}

void WaveData::setY(int i, double y)
{
    if (realTimeQuantify)
    {
        if (volQuantiLevel > 0)
            y = 1.0*qRound(y*volQuantiLevel)/volQuantiLevel;
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
        if (unitTime > 0)
            x = 1.0*qRound(x/unitTime)*unitTime;
        if (volQuantiLevel > 0)
            y = 1.0*qRound(y*volQuantiLevel)/volQuantiLevel;
    }
    dataX.insert(i, x);
    dataY.insert(i, y);
}

void WaveData::save()
{
    if (historyX.count() > 0 && dataX == historyX.at(point) && dataY == historyY.at(point) && unitTime == historyMDT.at(point) && volQuantiLevel == historyVQL.at(point) && maxVol == historyMaxVol.at(point))
        return;
    while (point < historyX.count()-1)
    {
        historyX.removeLast();
        historyY.removeLast();
        historyMDT.removeLast();
        historyVQL.removeLast();
        historyMaxVol.removeLast();
    }
    historyX.append(dataX);
    historyY.append(dataY);
    historyMDT.append(unitTime);
    historyVQL.append(volQuantiLevel);
    historyMaxVol.append(maxVol);
    point++;
}

void WaveData::unDo()
{
    point--;
    dataX = historyX.at(point);
    dataY = historyY.at(point);
    unitTime = historyMDT.at(point);
    volQuantiLevel = historyVQL.at(point);
    maxVol = historyMaxVol.at(point);
}

int WaveData::countUnDo()
{
    return point;
}

void WaveData::reDo()
{
    point++;
    dataX = historyX.at(point);
    dataY = historyY.at(point);
    unitTime = historyMDT.at(point);
    volQuantiLevel = historyVQL.at(point);
    maxVol = historyMaxVol.at(point);
}

int WaveData::countReDo()
{
    return historyX.count()-point-1;
}

void WaveData::drop()
{
    dataX = historyX.at(point);
    dataY = historyY.at(point);
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

void WaveData::setVolQuantiLevel(int level)
{
    volQuantiLevel = level;
    quantify();
}

int WaveData::getVolQuantiLevel()
{
    return volQuantiLevel;
}

void WaveData::setUnitTime(double time)
{
    unitTime = time;
    quantify();
}

double WaveData::getUnitTime()
{
    return unitTime;
}

void WaveData::setMaxVol(double vol)
{
    maxVol = vol;
    quantify();
}

double WaveData::getMaxVol()
{
    return maxVol;
}

void WaveData::quantify(double dx, int level, bool enforce)
{
    int i;
    if(dx > 0)
        unitTime = dx;
    if(level > 0)
        volQuantiLevel = level;

    if (!realTimeQuantify && !enforce)
        return;

    if (unitTime > 0 && dataX.count() > 1)
        for (i = 1; i < dataX.count(); i++)
            setX(i, 1.0*qRound(dataX.at(i)/unitTime)*unitTime);

    if ((1.0/volQuantiLevel) > 0 && dataY.count() > 1)
        for (i = 0; i < dataY.count(); i++)
            setY(i, 1.0*qRound(dataY.at(i)*volQuantiLevel)/volQuantiLevel);
}
