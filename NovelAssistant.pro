#-------------------------------------------------
#
# Project created by QtCreator 2018-10-22T10:50:22
#
#-------------------------------------------------

QT       += core gui
QT       += xml
QT       += sql widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = WsNA
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    sqlport.cpp \
    components.cpp \
    locationedit.cpp \
    propedit.cpp \
    skilledit.cpp \
    characteredit.cpp \
    superdatetool.cpp

HEADERS += \
        mainwindow.h \
    sqlport.h \
    components.h \
    locationedit.h \
    propedit.h \
    skilledit.h \
    characteredit.h \
    superdatetool.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    小说控制模型.md \
    README.md \
    WsNovelAssistant.epgz \
    NovelAssistant基本策划.docx
