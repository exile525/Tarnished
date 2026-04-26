QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    aichat.cpp \
    main.cpp \
    monitor.cpp \
    widget.cpp

HEADERS += \
    aichat.h \
    fsmpBeeper.h \
    fsmpCamera.h \
    fsmpEvents.h \
    fsmpFan.h \
    fsmpLed.h \
    fsmpProximity.h \
    fsmpTempHum.h \
    monitor.h \
    widget.h

FORMS += \
    aichat.ui \
    monitor.ui \
    widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    tupian.qrc

DISTFILES +=
