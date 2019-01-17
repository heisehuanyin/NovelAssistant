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
    storyboard.cpp \
    storydisplay.cpp \
    superdatetool.cpp \
    docmanager.cpp \
    edserver.cpp \
    projectsymbo.cpp \
    hideidmodel.cpp \
    frontend.cpp \
    character.cpp \
    eventnodes.cpp \
    location.cpp \
    items.cpp \
    ability.cpp \
    typekindgrade.cpp \
    queryutility.cpp \
    syntaxhightlighter.cpp \
    dbtool.cpp

HEADERS += \
    storyboard.h \
    storydisplay.h \
    superdatetool.h \
    docmanager.h \
    edserver.h \
    projectsymbo.h \
    frontend.h \
    character.h \
    eventnodes.h \
    location.h \
    items.h \
    ability.h \
    typekindgrade.h \
    hideidmodel.h \
    queryutility.h \
    syntaxhightlighter.h \
    dbtool.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    小说控制模型.md \
    README.md \
    WsNovelAssistant.epgz \
    NovelAssistant基本策划.docx \
    ExampleProject.xml \
    NovelAssistant基本策划 - 1.0基本.docx \
    NovelAssistant基本策划 - 2.0增改.docx
