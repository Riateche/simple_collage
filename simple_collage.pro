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
    CropHandle.cpp \
    Graphics_view_zoom.cpp

HEADERS  += MainWindow.h \
    CropHandle.h \
    Graphics_view_zoom.h

FORMS    += MainWindow.ui \
    CropForm.ui
