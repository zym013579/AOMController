#ifndef WAVEDATA_H
#define WAVEDATA_H

#include <QList>

/**
 * @brief 一张图表对应一个WaveData，函数均不检验输入是否正确，若出错则为程序逻辑错误
 */
class WaveData
{
public:
    WaveData();
    ~WaveData();

    /**
     * @brief 输出当前x轴数据
     * @return x轴数据
     */
    QList<double> x();

    /**
     * @brief 输出当前y轴数据
     * @return y轴数据
     */
    QList<double> y();

    /**
     * @brief 在末尾新增点数据
     * @param x轴数据
     * @param y轴数据
     */
    void add(double x, double y);

    void add(QList<double> x, QList<double> y);

    /**
     * @brief 清除所有数据
     */
    void clear();

    /**
     * @brief 复制输入变量的当前图表数据到当前图表
     * @param data 输入数据
     */
    void copyData(WaveData *data);

    /**
     * @brief 统计点个数
     * @return 点个数
     */
    int count();

    /**
     * @brief 某个点的x轴数据
     * @param i 点序号
     * @return 某个点的x轴数据
     */
    double xAt(int i);

    /**
     * @brief 某个点的y轴数据
     * @param i 点序号
     * @return 某个点的y轴数据
     */
    double yAt(int i);

    /**
     * @brief 更改某个点的数据
     * @param i 点序号
     * @param x x轴
     * @param y y轴
     */
    void set(int i, double x, double y);

    /**
     * @brief 更改某个点的数据
     * @param i 点序号
     * @param x x轴
     */
    void setX(int i, double x);

    /**
     * @brief 更改某个点的数据
     * @param i 点序号
     * @param y y轴
     */
    void setY(int i, double y);

    /**
     * @brief 删除某个点
     * @param i 点序号
     */
    void del(int i);

    /**
     * @brief 插入一个点
     * @param i 点序号(i之后)
     * @param x x轴
     * @param y y轴
     */
    void insert(int i, double x, double y);

    /**
     * @brief 存储当前状态，(若与上个状态相同则不存储)
     */
    void save();

    /**
     * @brief 撤销一步
     */
    void unDo();

    /**
     * @brief 能够撤销的步数
     * @return 步数
     */
    int countUnDo();

    /**
     * @brief 还原一步
     */
    void reDo();

    /**
     * @brief 能够还原的步数
     * @return 步数
     */
    int countReDo();

    /**
     * @brief 放弃当前图像数据(恢复至更改前，一般用于丢弃无需保存实时显示数据)
     */
    void drop();

    void setRealTimeQuantify(bool enabled);

    bool getRealTimeQuantify();

    void setVolQuantiLevel(int level);

    int getVolQuantiLevel();

    void setUnitTime(double time);

    double getUnitTime();

    void setMaxVol(double vol);

    double getMaxVol();

    void quantify(double dx = -1, int level = -1, bool enforce = false);

protected:
    int point;

    bool realTimeQuantify;

    int volQuantiLevel;

    double unitTime, maxVol;

    QList<double> dataX, dataY;

    QList<QList<double>> historyX, historyY;

    QList<int> historyVQL;

    QList<double> historyMDT, historyMaxVol;
};

#endif // WAVEDATA_H
