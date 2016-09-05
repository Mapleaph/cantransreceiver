#ifndef COMMON_H
#define COMMON_H

#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include "canctl.h"
#include <QDialog>

#define MAX_DLC 8

#define DBG 0

static QString ff_str[2] = {"Standard", "Extended"};

struct Settings {
    QString device;
    QString baudrate;
    qint8 ff;
};

struct OriPacket {
    qint32 dlc;
    qint32 id;
    qint32 *data;
    qint32 ff;
};

#endif // COMMON_H

