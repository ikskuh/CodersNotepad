#-------------------------------------------------
#
# Project created by QtCreator 2015-05-29T16:04:28
#
#-------------------------------------------------

QT       += core gui

CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CodersNotepad
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    codeeditor.cpp \
    language.cpp \
    Languages/simplelanghighlighter.cpp \
    Languages/simplelanguage.cpp

HEADERS  += mainwindow.h \
    codeeditor.h \
    language.h \
    Languages/simplelanghighlighter.h \
    Languages/simplelanguage.h

FORMS    +=

DISTFILES += \
    plan.txt
