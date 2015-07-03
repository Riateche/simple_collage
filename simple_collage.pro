#-------------------------------------------------
#
# Project created by QtCreator 2014-07-17T20:48:19
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = simple_collage
TEMPLATE = app


SOURCES += main.cpp\
        MainWindow.cpp \
    CropForm.cpp \
    Resizable_rubber_band.cpp

HEADERS  += MainWindow.h \
    CropForm.h \
    Resizable_rubber_band.h

FORMS    += MainWindow.ui \
    CropForm.ui
