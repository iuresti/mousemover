#-------------------------------------------------
#
# Project created by QtCreator 2015-10-26T11:31:46
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MouseMover
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    aboutdialog.cpp

HEADERS  += mainwindow.h \
    aboutdialog.h \
    defs.h

FORMS    += mainwindow.ui \
    aboutdialog.ui

RESOURCES += \
    mousemover.qrc
