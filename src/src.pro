TEMPLATE = app
DESTDIR = ../bin
TARGET = sigviewer
CONFIG += warn_on \
    link_prl \
    qt \
    thread
macx:QT += opengl
OBJECTS_DIR = ../tmp/release
MOC_DIR = ../tmp/release
RCC_DIR = ../tmp/release
UI_DIR = ../tmp/release
DEFINES += QT_NO_DEBUG_OUTPUT

CONFIG(release) {
    CONFIG -= debug
}

CONFIG(debug) {
    OBJECTS_DIR = ../tmp/debug
    MOC_DIR = ../tmp/debug
    RCC_DIR = ../tmp/debug
    UI_DIR = ../tmp/debug
    DEFINES -= QT_NO_DEBUG_OUTPUT
}

INCLUDEPATH += ../extern
INCLUDEPATH += .

LIBS += -L../extern \
    -L../../trunk/extern \
    -lbiosig \
    -lfftw3
win32:LIBS += -lws2_32

macx:LIBS += -lz
RESOURCES = src.qrc
win32:RC_FILE = src.rc
ICON = sigviewer.icns
TRANSLATIONS += translations\sigviewer_de.ts \
    translations\sigviewer_en.ts \
    translations\sigviewer_es.ts \
    translations\sigviewer_fr.ts
include(base/base.pri)
include(signal_processing/signal_processing.pri)
include(file_handling_impl/file_handling_impl.pri)
include(file_handling/file_handling.pri)
include(gui/gui.pri)
include(gui_impl/gui_impl.pri)
include(editing_commands/editing_commands.pri)
include(tests/tests.pri)
HEADERS += application_context_impl.h \
    file_context.h \
    tab_context.h \
    command_executer.h
SOURCES += main/sigviewer.cpp \
    application_context_impl.cpp \
    file_context.cpp \
    tab_context.cpp
