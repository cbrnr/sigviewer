TEMPLATE = app
DESTDIR = $$_PRO_FILE_PWD_/../bin
TARGET = sigviewer

QT += core gui xml

CONFIG(debug, debug|release) {
    message(DEBUG)
    DESTDIR = $$_PRO_FILE_PWD_/../bin/debug
    DEFINES -= QT_NO_DEBUG_OUTPUT
    OBJECTS_DIR = $$_PRO_FILE_PWD_/../tmp/debug
    MOC_DIR = $$_PRO_FILE_PWD_/../tmp/debug
    RCC_DIR = $$_PRO_FILE_PWD_/../tmp/debug
    UI_DIR = $$_PRO_FILE_PWD_/../tmp/debug
}
CONFIG(release, debug|release) {
    message(RELEASE)
    DESTDIR = $$_PRO_FILE_PWD_/../bin/release
    DEFINES += QT_NO_DEBUG_OUTPUT
    OBJECTS_DIR = $$_PRO_FILE_PWD_/../tmp/release
    MOC_DIR = $$_PRO_FILE_PWD_/../tmp/release
    RCC_DIR = $$_PRO_FILE_PWD_/../tmp/release
    UI_DIR = $$_PRO_FILE_PWD_/../tmp/release
}

CONFIG += warn_on \
    link_prl \
    qt \
    thread

macx:QT += opengl

INCLUDEPATH += $$_PRO_FILE_PWD_/../extern/include \
               $$_PRO_FILE_PWD_/.
LIBS += -L$$_PRO_FILE_PWD_/../extern/lib \
    -lbiosig# \
    #-lGDF

macx:INCLUDEPATH += /opt/local/include
macx:LIBS += -L/opt/local/lib \
             -lcholmod \
             -lz

RESOURCES = src.qrc
win32:RC_FILE = src.rc
ICON = sigviewer.icns
TRANSLATIONS += translations\sigviewer_de.ts \
    translations\sigviewer_en.ts \
    translations\sigviewer_es.ts \
    translations\sigviewer_fr.ts
#include(../extern/extern.pri)
include(base/base.pri)
include(signal_processing/signal_processing.pri)
include(file_handling_impl/file_handling_impl.pri)
include(file_handling/file_handling.pri)
include(gui/gui.pri)
include(gui_impl/gui_impl.pri)
include(editing_commands/editing_commands.pri)
#include(console/console.pri)
#include(console_impl/console_impl.pri)
include(commands/commands.pri)
include(tests/tests.pri)
HEADERS += application_context_impl.h \
    file_context.h \
    tab_context.h \
    command_executer.h
SOURCES += main/sigviewer.cpp \
    application_context_impl.cpp \
    file_context.cpp \
    tab_context.cpp
