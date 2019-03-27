#-------------------------------------------------
#
# Project created by QtCreator 2018-10-31T15:14:30
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = rtvolcano_slavnov_n
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#QMAKE_CXXFLAGS += -Wc++11-extensions

CONFIG += c++14


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    rtvector.cpp \
    rtrender.cpp \
    rtpoint.cpp \
    rtray.cpp \
    rtvoxel.cpp \
    rtscene.cpp \
    rtcanvasqt.cpp \
    rtcamera.cpp \
    rtflow.cpp \
    rtcolor.cpp

HEADERS += \
        mainwindow.h \
    rtpoint.h \
    rtray.h \
    rtvector.h \
    rtrender.h \
    rtcanvas.h \
    rtvoxel.h \
    rtscene.h \
    rtcanvasqt.h \
    rtcamera.h \
    rtflow.h \
    rtcolor.h

FORMS += \
        mainwindow.ui
