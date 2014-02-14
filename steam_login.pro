#-------------------------------------------------
#
# Project created by QtCreator 2014-02-14T18:01:06
#
#-------------------------------------------------

QT       += core network qml

QT       -= gui

TARGET = steam_login
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    worker.cpp \
    core.cpp \
    logger.cpp \
    util.cpp

QMAKE_CXXFLAGS += -std=c++0x

unix:!mac{

  QMAKE_LFLAGS += -Wl,--rpath=\\\$\$ORIGIN

  QMAKE_LFLAGS += -Wl,--rpath=\\\$\$ORIGIN/lib

  QMAKE_LFLAGS += -Wl,--rpath=\\\$\$ORIGIN/libs

  QMAKE_RPATH=

}

HEADERS += \
    worker.h \
    core.h \
    logger.h \
    util.h

RESOURCES += \
    resources.qrc
