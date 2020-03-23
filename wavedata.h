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
    static QVector<double> ModuX, ModuY;
    static QVector<double> EditX, EditY;
    static double deltaX;
    static double voltage;
};

#endif // WAVEDATA_H
