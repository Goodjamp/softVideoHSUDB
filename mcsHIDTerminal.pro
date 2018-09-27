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
    mcstransportclass.cpp \
    mcsprotocolclass.cpp \
    hidinterface.cpp \
    communicationclass.cpp

HEADERS += mainwindow.h \
           mainParDescriptions.h \
    mcstransportclass.h \
    mcsprotocolclass.h \
    hidinterface.h \
    communicationclass.h

FORMS   += mainwindow.ui

LIBS    += -lhid\
           -lsetupapi

CONFIG  += c++11
