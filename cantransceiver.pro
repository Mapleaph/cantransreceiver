#-------------------------------------------------
#
# Project created by QtCreator 2016-08-18T14:33:06
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


QMAKE_CXXFLAGS += -std=c++11

TARGET = cantransceiver
TEMPLATE = app

SOURCES += main.cpp\
        dialog.cpp \
    configdialog.cpp \
    worker.cpp

HEADERS  += dialog.h \
    configdialog.h \
    worker.h \
    common.h \
    canctl.h

FORMS    += dialog.ui \
    configdialog.ui
