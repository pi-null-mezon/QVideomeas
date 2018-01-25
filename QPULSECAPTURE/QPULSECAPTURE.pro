#-------------------------------------------------
#
# Project created by QtCreator 2014-02-08T18:00:16
#
#-------------------------------------------------

QT +=   core widgets gui serialport multimedia

TEMPLATE =  app
TARGET  =   QPULSECAPTURE # legacy name for compatibility with Maximus
VERSION =   5.0.0.0

DEFINES +=  APP_NAME=\\\"$${TARGET}\\\" \
            APP_VERS=\\\"$${VERSION}\\\"

SOURCES +=  main.cpp\
            mainwindow.cpp \
            qimagewidget.cpp \
            qopencvprocessor.cpp \
            qvideocapture.cpp \
            ../Oscilloscope/qeasyplot.cpp \
            vpglibinterface/qpulseprocessor.cpp \
            ../Oscilloscope/qtemperatureprocessor.cpp \
            ../Oscilloscope/qserialportprocessor.cpp

HEADERS  += mainwindow.h \
            qimagewidget.h \
            qopencvprocessor.h \
            qvideocapture.h \
            ../Oscilloscope/qeasyplot.h \
            vpglibinterface/qpulseprocessor.h \
            ../Oscilloscope/qtemperatureprocessor.h \
            ../Oscilloscope/qserialportprocessor.h

INCLUDEPATH += ../Oscilloscope \
               ../Seriesanalysis \
               vpglibinterface

DEPENDPATH += ../Oscilloscope \
              ../Seriesanalysis \
              vpglibinterface

include(opencv.pri)
include(opengl.pri)
include(C:/Programming/vpglib/Shared/vpglib.pri)

CONFIG += designbuild
designbuild {
    message("Design build configuration was selected, design paths will be used")
    DEFINES += HAARCASCADES_PATH=\\\"$${OPENCV_DIR}/../sources/data/haarcascades/\\\" \
               LBPCASCADES_PATH=\\\"$${OPENCV_DIR}/../sources/data/lbpcascades/\\\"
} else {
    message("Release build configuration was selected, deployment paths will be used")
}

TRANSLATIONS += $${TARGET}.ts
RC_ICONS = ../Resources/degrees.ico




