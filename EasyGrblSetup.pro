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

SOURCES += source/main.cpp \
           source/EasyGrblSetup.cpp \
           source/ConfigWatcher.cpp \
           source/GrblCommander.cpp \
           source/JogController.cpp \
           source/AxesVisibility.cpp

HEADERS += source/EasyGrblSetup.h \
           source/qt_before_5_7_0.h \
           source/QLabelClickable.h \
           source/ConfigWatcher.h \
           source/GrblCommander.h \
           source/JogController.h \
           source/AxesVisibility.h

FORMS += source/EasyGrblSetup.ui

TRANSLATIONS += locale/EasyGrblSetup_fr.ts \
                locale/EasyGrblSetup_de.ts
