#ifndef WAVEDATA_H
#define WAVEDATA_H

//define WAVELENGTH 100001

#include <QList>

class WaveData
{
public:
    //static double InitX[WAVELENGTH], InitY[WAVELENGTH];
    //static double ModuX[WAVELENGTH], ModuY[WAVELENGTH];
    //static double EditX[WAVELENGTH], EditY[WAVELENGTH];
    /*
    static QVector<double> moduX, moduY;
    static QVector<double> editX, editY;
    static QVector<double> tempX, tempY;
    static double deltaX;
    static double voltage;
    */
    void init()
    {
        this->clear();
        dataX.append(0);
        dataY.append(0);
        this->save();
    }

    QList<double> x()
    {
        return dataX;
    }

    QList<double> y()
    {
        return dataY;
    }

    void add(double x, double y)
    {
        this->save();
        dataX.append(x);
        dataY.append(y);
    }

    void clear()
    {
        dataX.clear();
        dataY.clear();
        historyX.clear();
        historyY.clear();
        futureX.clear();
        futureY.clear();
    }

    void copyData(WaveData *data)
    {
        this->save();
        dataX.clear();
        dataY.clear();
        dataX.append(data->x());
        dataY.append(data->y());
    }

    int count()
    {
        return dataX.size();
    }

    double x_at(int i)
    {
        return dataX.at(i);
    }

    double y_at(int i)
    {
        return dataY.at(i);
    }

    void set(int i, double x, double y, bool if_save)
    {
        if (if_save)
            this->save();
        dataX[i] = x;
        dataY[i] = y;
    }

    void del(int i)
    {
        this->save();
        dataX.removeAt(i);
        dataY.removeAt(i);
    }

    void insert(int i, double x, double y)
    {
        this->save();
        dataX.insert(i, x);
        dataY.insert(i, y);
    }

    void save()
    {
        if (historyX.count() > 0 && dataX == historyX.last() && dataY == historyY.last())
            return;
        historyX.append(dataX);
        historyY.append(dataY);
        futureX.clear();
        futureY.clear();
    }

    void unDo()
    {
        futureX.append(dataX);
        futureY.append(dataY);
        dataX = historyX.last();
        dataY = historyY.last();
        historyX.removeLast();
        historyY.removeLast();
    }

    int count_unDo()
    {
        return historyX.count();
    }

    void reDo()
    {
        historyX.append(dataX);
        historyY.append(dataY);
        dataX = futureX.last();
        dataY = futureY.last();
        futureX.removeLast();
        futureY.removeLast();
    }

    int count_reDo()
    {
        return futureX.count();
    }
protected:
    QList<double> dataX, dataY;

    QList<QList<double>> historyX, historyY, futureX, futureY;
};

#endif // WAVEDATA_H
