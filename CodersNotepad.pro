#-------------------------------------------------
#
# Project created by QtCreator 2015-05-29T16:04:28
#
#-------------------------------------------------

QT       += core gui
QT		 += webkitwidgets

CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CodersNotepad
TEMPLATE = app

webkitwidgets {
    DEFINES += HAS_WEBBROWSER
}

SOURCES += main.cpp\
        mainwindow.cpp \
    codeeditor.cpp \
    language.cpp \
    Languages/simplelanguage.cpp \
    webbrowser.cpp \
    generichighlighter.cpp

HEADERS  += mainwindow.h \
    codeeditor.h \
    language.h \
    Languages/simplelanguage.h \
    webbrowser.hpp \
    generichighlighter.hpp

FORMS    +=

DISTFILES += \
    plan.txt

RESOURCES += \
    icons.qrc
