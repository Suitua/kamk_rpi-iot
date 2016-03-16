TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt
SOURCES += main.cpp \
    ../Arduino_Node/thingspeakrequest.cpp \
    ../src/fifobuffer.cpp \
    ../src/httpclient.cpp \
    ../src/pmutex.cpp \
    ../src/strlib.cpp \
    ../src/tcpsocket.cpp \
    ../src/thingspeakclient.cpp \
    ../src/armPi/rf24receiver.cpp \
    ../src/thread.cpp



target.path = /home/pi/bin
INSTALLS += target

LIBS += -lrt -lpthread -lrf24-bcm

INCLUDEPATH += ../src \
               ../Arduino_Node

HEADERS += \
    ../Arduino_Node/thingspeakrequest.h \
    ../src/fifobuffer.h \
    ../src/httpclient.h \
    ../src/pmutex.h \
    ../src/strlib.h \
    ../src/tcpsocket.h \
    ../src/thingspeakclient.h \
    ../src/armPi/rf24receiver.h \
    ../src/thread.h

DISTFILES += \
    ../src/LICENSE.txt


