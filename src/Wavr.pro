#-------------------------------------------------
#
# Project created by QtCreator 2013-01-25T17:45:45
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Wavr
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    QProgressIndicator.cpp \
    tcpnetwork.cpp \
    network.cpp \
    udpnetwork.cpp \
    channelMessaging.cpp \
    datagram.cpp \
    shared.cpp

HEADERS  += mainwindow.h \
    QProgressIndicator.h \
    tcpnetwork.h \
    shared.h \
    definitions.h \
    network.h \
    udpnetwork.h \
    settings.h \
    channelMessaging.h \
    datagram.h

FORMS    += \
    mainwindow.ui
