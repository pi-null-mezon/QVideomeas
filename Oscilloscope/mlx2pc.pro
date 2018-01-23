#-------------------------------------------------
#
# Project created by QtCreator 2014-12-08T23:26:24
#
#-------------------------------------------------

QT       += core \
            gui \
            serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = mlx2pc
VERSION = 1.0.0.2
TEMPLATE = app

DEFINES =   APP_AUTHOR=\\\"Alex_A._Taranov\\\"  \
            APP_COMPANY=\\\"Moscow_BMSTU\\\"    \
            APP_RELEASE_YEAR=\\\"2014\\\"       \
            APP_NAME=\\\"$${TARGET}\\\"         \
            APP_VERSION=\\\"$${VERSION}\\\"

SOURCES +=  main.cpp\
            mainwindow.cpp \
            qeasyplot.cpp \
            qserialprocessor.cpp \
            qmeasurementstreamprocessor.cpp

HEADERS  += mainwindow.h \
            qeasyplot.h \
            qserialprocessor.h \
            qmeasurementstreamprocessor.h

CONFIG(release, debug|release): DEFINES += QT_NO_WARNING_OUTPUT

RC_ICONS = $${PWD}/../Resources/degrees.ico


