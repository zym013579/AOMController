#ifndef COMMAND_H
#define COMMAND_H

#include <QMessageBox>

//一些全局使用的常用函数

#define MSG_WARNING(message) (QMessageBox::warning(this, "提示", message, QMessageBox::Ok, QMessageBox::Ok))
#define MAX(a,b)  (((a)>(b))?(a):(b))
#define MIN(a,b)  (((a)<(b))?(a):(b))

#endif // COMMAND_H
