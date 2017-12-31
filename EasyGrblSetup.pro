#-------------------------------------------------
#
# Project created by QtCreator 2017-12-19T20:37:25
#
#-------------------------------------------------

QT       += core gui
QT       += serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = EasyGrblSetup
TEMPLATE = app

CONFIG += c++11

SOURCES += main.cpp\
           mainwindow.cpp \
    ConfigWatcher.cpp

HEADERS  += mainwindow.h \
    ConfigWatcher.h

FORMS    += mainwindow.ui
