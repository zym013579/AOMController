#ifndef SIGNAL_H
#define SIGNAL_H

//#define R_TTL 0x00
//#include <QMessageBox>

//下位机通信字符

//发送开始调制命令
#define SEND_START "(1)"
//发送暂停调制命令
#define SEND_PAUSE "(2)"
//测试连接状态命令
#define SEND_CONNECTION_TEST "(3)"

//指定从下位机接收信息的字符数目的最大最小值
#define RECV_MAX_LENGTH 1
#define RECV_MIN_LENGTH 1
//指令或信息接收成功时，下位机返回的字符
#define RECV_OK "~"

//默认/最大电压量化位数(正整数)
#define DEFAULT_VOL_QUANTIFY_LEVEL 4096-1   //2^14-1
#define MAX_VOL_QUANTIFY_LEVEL 4096-1

//最小单位时间，需写成*实数*，不然会发生魔幻问题(被当成整数处理)
#define DEFAULT_UNIT_TIME 1.0
#define DEFAULT_REALTIME_QUANTIFY true

//QByteArray数据类型的最大字符数，实测为9999
//目前编码之后超过9990字符的信息会被分批发送
#define QByteArray_MAX_NUM 9999

/*修改串口参数
 *
 * 该部分在mainwindow.cpp中
 * setPortConfig()函数中
 */

#endif // SIGNAL_H
