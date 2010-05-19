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
           ../src/signal_processing/*.cc \
           ../src/editing_commands/*.cpp \
           ../src/file_handling/*.cpp \
           ../src/file_handling_impl/*.cpp \
           ../src/gui/*.cpp \
           ../src/gui_impl/*.cpp \
           ../src/gui_impl/dialogs/*.cpp \
           ../src/signal_browser/*.cpp \
           ../src/*.cpp \
           *.cpp
HEADERS += base_tests/*.h \
           ../src/signal_processing/*.h \
           ../src/base/*.h \
           ../src/editing_commands/*.h \
           ../src/file_handling/*.h \
           ../src/file_handling_impl/*.h \
           ../src/gui/*.h \
           ../src/gui_impl/*.h \
           ../src/gui_impl/dialogs/*.h \
           ../src/signal_browser/*.h \
           ../src/*.h \
           file_handling_tests/*.h
FORMS += ../src/gui_impl/dialogs/*.ui \
    ../src/signal_browser/*.ui
