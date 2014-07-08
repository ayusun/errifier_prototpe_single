#-------------------------------------------------
#
# Project created by QtCreator 2014-07-05T20:36:49
#
#-------------------------------------------------

QT       += core gui

TARGET = errifier
TEMPLATE = app


SOURCES += main.cpp\
        errifiergui.cpp

HEADERS  += errifiergui.h \
    common.h

FORMS    += errifiergui.ui

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += gtk+-2.0

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += glib-2.0 libnotify

RESOURCES     = errifier.qrc
