# -------------------------------------------------
# Project created by QtCreator 2010-03-28T07:16:55
# -------------------------------------------------
QT += testlib
TARGET = testframework
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app

INCLUDEPATH += ../extern
LIBS += -L../extern \
    -lbiosig \
    -lfftw3

include(filehandling_mockup/filehandling_mockup.pri)


SOURCES += main.cpp
