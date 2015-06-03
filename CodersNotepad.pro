#-------------------------------------------------
#
# Project created by QtCreator 2015-05-29T16:04:28
#
#-------------------------------------------------

QT       += core gui
QT		 += webkitwidgets xml

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
    webbrowser.cpp \
    generichighlighter.cpp \
    genericlanguage.cpp \
    tool.cpp

HEADERS  += mainwindow.h \
    codeeditor.h \
    language.h \
    webbrowser.hpp \
    generichighlighter.hpp \
    genericlanguage.hpp \
    tool.h

FORMS    +=

DISTFILES += \
    plan.txt \
	Languages/cpp.xml \
    Languages/lua.xml \
    Tools/gamestudio.xml \
    Tools/lua-win.xml \
    Tools/lua-unix.xml \
    Languages/vala.xml \
    Tools/valac.xml \
    Tools/vala.xml

RESOURCES += \
    icons.qrc
