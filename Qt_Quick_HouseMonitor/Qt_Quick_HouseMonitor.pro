TEMPLATE = app

QT += qml quick
CONFIG += c++11

SOURCES += main.cpp \
    ../src/strlib.cpp \
    ../src/httpclient.cpp \
    ../src/tcpsocket.cpp \
    ../src/thingspeakclient.cpp \
    thingspeakreader.cpp \
    ../src/qthingspeakreader.cpp

RESOURCES += qml.qrc


target.path = /home/pi/SMTL

INSTALLS += target


# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)


INCLUDEPATH += ../src

HEADERS += \
    ../src/strlib.h \
    ../src/httpclient.h \
    ../src/tcpsocket.h \
    ../src/thingspeakclient.h \
    thingspeakreader.h \
    ../src/qthingspeakreader.h


