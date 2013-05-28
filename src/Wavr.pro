#-------------------------------------------------
#
# Project created by QtCreator 2013-01-25T17:45:45
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Wavr
TEMPLATE = app

QT += xml

SOURCES += main.cpp\
        mainwindow.cpp \
    QProgressIndicator.cpp \
    libWavr/network.cpp \
    libWavr/udpnetwork.cpp \
    libWavr/tcpnetwork.cpp \
    libWavr/channelMessaging.cpp \
    libWavr/shared.cpp \
    libWavr/datagram.cpp \
    libWavr/xmlmessage.cpp \
    libWavr/messaging.cpp \
    libWavr/messagingproc.cpp \
    core.cpp \
    chatwindow.cpp \
    messagelog.cpp \
    chathelper.cpp \
    usertreewidget.cpp

HEADERS  += mainwindow.h \
    QProgressIndicator.h \
    definitions.h \
    libWavr/network.h \
    libWavr/udpnetwork.h \
    libWavr/tcpnetwork.h \
    libWavr/channelMessaging.h \
    libWavr/shared.h \
    libWavr/settings.h \
    libWavr/definitions.h \
    libWavr/datagram.h \
    libWavr/xmlmessage.h \
    libWavr/messaging.h \
    core.h \
    chatwindow.h \
    messagelog.h \
    uidefinitions.h \
    chathelper.h \
    chatdefinitions.h \
    usertreewidget.h \
    libWavr/stdlocation.h

FORMS    += \
    mainwindow.ui \
    chatwindow.ui

RESOURCES += \
    wavrIcons.qrc \
    wavrSounds.qrc

OTHER_FILES +=
