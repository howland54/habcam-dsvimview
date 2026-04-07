#-------------------------------------------------
#
# Project created by QtCreator 2017-08-18T12:12:57
#
#-------------------------------------------------

QT       += core gui printsupport network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

#DEFINES += ROS_OPENCV
DEFINES += NOROS


CONFIG += c++11

TARGET = vimview
TEMPLATE = app
INCLUDEPATH += /usr/local/include/
INCLUDEPATH += /usr/local/include/habcam-lcmtypes/
INCLUDEPATH += /usr/local/qwt-6.2.0/include
INCLUDEPATH += /usr/include/opencv4/
LIBS += -L/usr/lib
LIBS += -L/usr/local/qwt-6.2.0/lib/
LIBS += -L/usr/lib/x86_64-linux-gnu/
LIBS += -L/usr/local/lib
LIBS += -L../../dsvimlib/lib
LIBS += -llcm
LIBS += -ldsvimlib
LIBS += -lqwt

LIBS += -lopencv_core
LIBS += -lopencv_highgui
LIBS += -lopencv_imgcodecs
LIBS += -lopencv_imgproc
LIBS += -lopencv_videoio





SOURCES += main.cpp\
    SensorPage.cpp \
    SystemParameterControl.cpp \
        VimView.cpp \
    ImageAcquisitionThread.cpp \
    VimCameraControl.cpp \
    qcustomplot.cpp \
    RepetitionControlWidget.cpp \
    qledindicator.cpp \
    Stwinch.cpp

HEADERS  += VimView.h \
    ImageAcquisitionThread.h \
    SensorPage.h \
    SystemParameterControl.h \
    VimCameraControl.h \
    qcustomplot.h \
    RepetitionControlWidget.h \
    cameradefines.h \
    qledindicator.h \
    Stwinch.h

RESOURCES += \
    style.qrc
