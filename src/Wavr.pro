#-------------------------------------------------
#
# Project created by QtCreator 2013-01-25T17:45:45
#
#-------------------------------------------------

QT += core gui network webkitwidgets xml

OUTPUT += console
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Wavr
TEMPLATE = app

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
    usertreewidget.cpp \
    libWavr/message.cpp \
    libWavr/settings.cpp \
    strings.cpp

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
    libWavr/stdlocation.h \
    libWavr/message.h \
    strings.h

FORMS    += \
    mainwindow.ui \
    chatwindow.ui

RESOURCES += \
    wavrIcons.qrc \
    wavrSounds.qrc

OTHER_FILES +=

win32: CONFIG(release, debug|release): LIBS += -L$$PWD/../wavrapp/lib/ -lwavrapp2
else:win32: CONFIG(debug, debug|release): LIBS += -L$$PWD/../wavrapp/lib/ -lwavrappd2
unix:!symbian: LIBS += -L$$PWD/../wavrapp/lib/ -lwavrapp

INCLUDEPATH += $$PWD/../wavrapp/include
DEPENDPATH += $$PWD/../wavrapp/include
