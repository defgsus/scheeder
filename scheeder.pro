#-------------------------------------------------
#
# Project created by QtCreator 2014-06-01T18:29:46
#
#-------------------------------------------------

TARGET = scheeder
TEMPLATE = app


QMAKE_CXXFLAGS += --std=c++0x

macx {
    QMAKE_CXXFLAGS += -std=c++11
    CONFIG += c++11
    INCLUDEPATH += /usr/local/include
}

QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets opengl

SOURCES += \
    main.cpp\
    mainwindow.cpp \
    renderwidget.cpp \
    basic3dwidget.cpp \
    sourcewidget.cpp \
    appsettings.cpp \
    model.cpp \
    modelfactory.cpp \
    debug.cpp \
    glsl.cpp \
    glslhighlighter.cpp \
    uniformwidgetfactory.cpp

HEADERS  += \
    mainwindow.h \
    renderwidget.h \
    basic3dwidget.h \
    vector.h \
    sourcewidget.h \
    appsettings.h \
    model.h \
    modelfactory.h \
    debug.h \
    glsl.h \
    opengl.h \
    glslhighlighter.h \
    uniformwidgetfactory.h \
    teapot_data.h

FORMS    += \
    mainwindow.ui
