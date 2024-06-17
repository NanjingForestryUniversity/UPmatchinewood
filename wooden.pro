#-------------------------------------------------
#
# Project created by QtCreator 2022-03-18T10:12:37
#
#-------------------------------------------------

QT       += core gui network serialbus serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = wooden
TEMPLATE = app

LIBS += -Ldll -lws2_32

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        widget.cpp \
    camera.cpp \
    thread.cpp \
    correct.cpp \
    src/modbus.c \
    src/modbus-data.c \
    src/modbus-rtu.c \
    src/modbus-tcp.c \
    showpic.cpp \
    relistwidget_item.cpp \
    imagewin.cpp

HEADERS += \
        widget.h \
    camera.h \
    thread.h \
    correct.h \
    src/modbus.h \
    src/modbus-private.h \
    src/modbus-rtu.h \
    src/modbus-rtu-private.h \
    src/modbus-tcp.h \
    src/modbus-tcp-private.h \
    src/modbus-version.h \
    showpic.h \
    relistwidget_item.h \
    imagewin.h

FORMS += \
        widget.ui \
    showpic.ui \
    imagewin.ui

#LIBS += -LC:/opencv/build/x64/vc15/lib -lopencv_world453

#INCLUDEPATH += C:/opencv/build/include \
#            += C:/opencv/build/include/opencv2

#LIBS += -LC:/Users/lzy/Downloads/opencv/build/x64/vc15/lib -lopencv_world453

#INCLUDEPATH += C:/Users/lzy/Downloads/opencv/build/include \
#            += C:/Users/lzy/Downloads/opencv/build/include/opencv2

LIBS += -LC:/Users/USER/Downloads/opencv/build/x64/vc15/lib -lopencv_world453

INCLUDEPATH += C:/Users/USER/Downloads/opencv/build/include \
            += C:/Users/USER/Downloads/opencv/build/include/opencv2

LIBS += -L'C:/Program Files/Teledyne DALSA/Sapera/Lib/Win64' -lSapClassBasic
LIBS += -L'C:/Program Files/Teledyne DALSA/Sapera/Lib/Win64' -lcorapi

INCLUDEPATH += 'C:/Program Files/Teledyne DALSA/Sapera/Include'
DEPENDPATH += 'C:/Program Files/Teledyne DALSA/Sapera/Include'
INCLUDEPATH += 'C:/Program Files/Teledyne DALSA/Sapera/Classes/Basic'
DEPENDPATH += 'C:/Program Files/Teledyne DALSA/Sapera/Classes/Basic'

DISTFILES += \
    src/modbus.lib




