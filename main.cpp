#include "dialog.h"
#include <QApplication>
#include "common.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Dialog w;
    w.show();

    qRegisterMetaType<Settings>("Settings");
    qRegisterMetaType<OriPacket>("OriPacket");
    qRegisterMetaType<CAN_MESSAGE>("CAN_MESSAGE");

    return a.exec();
}
