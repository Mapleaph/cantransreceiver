#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QEventLoop>
#include <QTimer>
#include <QTime>
#include <QDebug>
#include "common.h"
#include <pthread.h>

class Worker :public QObject
{
    Q_OBJECT
public:
    Worker();
    ~Worker();
    void initPort(qint32 canFD, qint32 baudrate, qint32 acc, qint32 acm);
    void intervalGen(qint32 interval);
    bool loopFlag;

signals:
    void sigExitThread();
    void sigOpened();
    void sigOpenFailed();
    void sigRecved(CAN_MESSAGE msg, qint32 elapsedTime);
    void sigClosed();

private slots:
    void doOpen(Settings currentSettings);
    void doSend(CAN_MESSAGE msg);
    void doLoopSend(CAN_MESSAGE msg, qint32 interval);
    void doClose();


};

#endif // WORKER_H
