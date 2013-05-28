QT       += network

TARGET = wavrapp
TEMPLATE = lib

DEFINES += WAVRAPP_LIBRARY

SOURCES += \
    qtsinglecoreapplication.cpp \
    qtsingleapplication.cpp \
    qtlockedfile_win.cpp \
    qtlockedfile_unix.cpp \
    qtlockedfile.cpp \
    qtlocalpeer.cpp \
    application.cpp

HEADERS += \
    qtsinglecoreapplication.h \
    qtsingleapplication.h \
    qtlockedfile.h \
    qtlocalpeer.h \
    application.h

CONFIG(debug, debug|release) {
        DESTDIR = ../lib
        win32: TARGET = $$join(TARGET,,,d)
        mac: TARGET = $$join(TARGET,,,_debug)
} else {
        DESTDIR = ../lib
}
