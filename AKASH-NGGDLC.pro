#-------------------------------------------------
#
# Project created by QtCreator 2018-01-27T13:25:51
#
#-------------------------------------------------

QT       += core gui
QT       += network
QT       += serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AKASH-NG-GDLC
TEMPLATE = app

CXXFLAGS += -std=gnu++11
CXXFLAGS += -fpermissive

SOURCES += main.cpp\
        mainwindow.cpp \
    radar.cpp \
    encoder.cpp \
    decoder.cpp

HEADERS  += mainwindow.h \
    radar.h \
    variables.h \
    encoder.h \
    decoder.h

FORMS    += mainwindow.ui

RESOURCES += \
    resources.qrc
