#-------------------------------------------------
#
# Project created by QtCreator 2018-03-15T10:12:28
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = mcsHIDTerminal
TEMPLATE = app


SOURCES += main.cpp\
           mainwindow.cpp \
    hidinterface.cpp \
    communicationclass.cpp \
    sendframetransportlclass.cpp \
    sendframeprotocolclass.cpp

HEADERS += mainwindow.h \
           mainParDescriptions.h \
    hidinterface.h \
    communicationclass.h \
    sendframeprotocolclass.h \
    sendframetransportlclass.h

FORMS   += mainwindow.ui

LIBS    += -lhid\
           -lsetupapi

CONFIG  += c++11
