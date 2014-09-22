#-------------------------------------------------
#
# Project created by QtCreator 2014-01-07T12:56:15
#
#-------------------------------------------------

QT       += core gui testlib

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Connector_Inspection_v2
TEMPLATE = app

CONFIG += console

INCLUDEPATH += "C:/Program Files/Allied Vision Technologies/AVTVimbaSDK_1.1" \
               "C:/opencv/build/include" \
               "C:/Program Files (x86)/LabJack/Drivers"\

DEFINES += NOMINMAX


SOURCES += main.cpp\
        mainwindow.cpp \
    Vimba_Wrapper.cpp \
    Measurement.cpp \
    LJ_Wrapper.cpp \
    LJ_Thread.cpp \
    FrameObserver.cpp \
    Camera_Thread.cpp

HEADERS  += mainwindow.h \
    Vimba_Wrapper.h \
    Measurement.h \
    LJ_Wrapper.h \
    LJ_Thread.h \
    FrameObserver.h \
    Camera_Thread.h

FORMS    += mainwindow.ui

QMAKE_LIBDIR += "C:/opencv/build/x86/vc10/lib" \
                "C:/Program Files/Allied Vision Technologies/AVTVimbaSDK_1.1/VimbaCPP/Lib/Win32" \
                "C:/Program Files (x86)/LabJack/Drivers"

LIBS += opencv_core246.lib opencv_imgproc246.lib opencv_highgui246.lib VimbaCPP.lib labjackud.lib

