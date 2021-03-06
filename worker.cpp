#include "worker.h"

qint32 canFD = -1;

volatile bool start_test_flag = true;

qint32 pid = -1;

pthread_t recv_thread;

void* recvPort(void *args)
{
    CAN_MESSAGE msg;
    Worker *tmpWorker = (Worker*)args;
    QTime t;

    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    while (start_test_flag) {

        t.start();

        if (::read(canFD, &msg, sizeof(msg)) > 0) {

            emit tmpWorker->sigRecved(msg, t.elapsed());

        }
    }

    pthread_exit(NULL);

    return NULL;
}


Worker::Worker()
{
    loopFlag = false;
}

Worker::~Worker()
{

}

void Worker::doOpen(Settings currentSettings)
{

    char *ch;
    QByteArray barr = currentSettings.device.toLatin1();
    ch = barr.data();

#if DBG
    qDebug() << currentSettings.baudrate;
    qDebug() << currentSettings.device;
    qDebug() << currentSettings.ff;
#endif

    canFD = ::open(ch, O_RDWR);
    qint32 baudrate = currentSettings.baudrate.toInt();
    qint32 acc = 0;
    qint32 acm = 0xffffffff;

    if (canFD < 0) {

        emit sigOpenFailed();

    } else {

        initPort(canFD, baudrate, acc, acm);



        start_test_flag = true;

        pid = pthread_create(&recv_thread, NULL, recvPort, this);

        emit sigOpened();

    }

}

void Worker::doClose()
{

    start_test_flag = false;

    intervalGen(100);

    pthread_cancel(recv_thread);

    pthread_join(recv_thread, NULL);

    ::close(canFD);

    emit sigExitThread();
}

void Worker::doSend(CAN_MESSAGE msg)
{

#if DBG
    qDebug() << msg.DataLenCode;
    qDebug() << msg.ID;
    qDebug() << msg.FrameFormat;
    qDebug() << msg.RemoteTransReq;

    for (qint8 i=0; i<msg.DataLenCode; i++) {
        qDebug() << msg.Data[i];
    }
#endif

    ::write(canFD, &msg, sizeof(msg));

}

void Worker::doLoopSend(CAN_MESSAGE msg, qint32 interval)
{

    while (loopFlag) {

        ::write(canFD, &msg, sizeof(msg));

        intervalGen(interval);

    }
}

void Worker::initPort(qint32 canFD, qint32 baudrate, qint32 acc, qint32 acm)
{
    ::ioctl(canFD, SET_CAN_BAUDRATE, &baudrate);
    ::ioctl(canFD, SET_CAN_ACC_CODE, &acc);
    ::ioctl(canFD, SET_CAN_ACM_CODE, &acm);
}

void Worker::intervalGen(qint32 interval)
{
    QEventLoop loop;
    QTimer::singleShot(interval, &loop, SLOT(quit()));
    loop.exec();
}
