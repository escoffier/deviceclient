#-------------------------------------------------
#
# Project created by QtCreator 2017-04-19T14:01:19
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = client
TEMPLATE = app
TRANSLATIONS+=cn.ts
#QMAKE_CXXFLAGS += -source-charset:utf-8
# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH +=".\rmi"
#LIBS += -L ".\rmi\" -l
SOURCES += main.cpp\
        mainwindow.cpp \
    pages.cpp \
    dialog.cpp \
    resourcepage.cpp \
    resourcemodel.cpp \
    treeitem.cpp \
    treemodel.cpp \
    devicetreemodel.cpp \
    configuredialog.cpp \
    querythread.cpp \
    treethread.cpp

HEADERS  += mainwindow.h \
    pages.h \
    dialog.h \
    resourepage.h \
    resourcemodel.h \
    treeitem.h \
    treemodel.h \
    devicetreemodel.h \
    configuredialog.h \
    querythread.h \
    treethread.h

RESOURCES += \
    clientimges.qrc \
    style.qrc



win32: LIBS += -L$$PWD/rmi/ -lrmi -lrmiClient -lapClient
win32: LIBS += -lWs2_32
#INCLUDEPATH += $$PWD/rmi
DEPENDPATH += $$PWD/rmi

FORMS += \
    configuredialog.ui
