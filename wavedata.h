#ifndef WAVEDATA_H
#define WAVEDATA_H

//define WAVELENGTH 100001

#include <QVector>

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
    QVector<double> x()
    {
        return dataX;
    }

    QVector<double> y()
    {
        return dataY;
    }
    void add(double x, double y)
    {
        dataX.append(x);
        dataY.append(y);
    }
    void clear()
    {
        dataX.clear();
        dataY.clear();
    }
    int count()
    {
        if (dataX.size() == dataY.size())
            return dataX.size();
        return -1;
    }
    double x_at(int i)
    {
        return dataX.at(i);
    }
    double y_at(int i)
    {
        return dataY.at(i);
    }
protected:
    QVector<double> dataX, dataY;
};

#endif // WAVEDATA_H
