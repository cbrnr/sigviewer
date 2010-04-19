# -------------------------------------------------
# Project created by QtCreator 2010-03-28T07:16:55
# -------------------------------------------------
QT += testlib
CONFIG += qtestlib \
          warn_on \
    link_prl \
    qt \
    thread \

debug {
    OBJECTS_DIR = ../tmp/debug
    MOC_DIR = ../tmp/debug
    RCC_DIR = ../tmp/debug
    UI_DIR = ../tmp/debug
}
INCLUDEPATH += ../extern \
    ../src
LIBS += -L../extern \
    -lbiosig \
    -lfftw3
#replace(include(../src/base/base.pri), "base/", "../src/base/")
#replace(include(../src/file_handling/file_handling.pri), "file_handling/", "../src/file_handling/")
#replace(include(../src/file_handling_impl/file_handling_impl.pri),"file_handling_impl/", "../src/file_handling_impl/")
#include(../src/file_handling/file_handling.pri)
#include(../src/file_handling_impl/file_handling_impl.pri)
SOURCES += base_tests/*.cpp \
           file_handling_tests/*.cpp \
           ../src/base/*.cpp \
           ../src/signal_processing/*.cpp \
           ../src/signal_processing/*.cc \
           ../src/file_handling/*.cpp \
           ../src/file_handling_impl/*.cpp \
           *.cpp
HEADERS += base_tests/*.h \
           ../src/signal_processing/*.h \
           ../src/base/*.h \
           ../src/file_handling/*.h \
           ../src/file_handling_impl/*.h \
           file_handling_tests/*.h
