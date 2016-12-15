TEMPLATE = app
DESTDIR = bin
TARGET = sigviewer

QT += core gui widgets xml

CONFIG(debug, debug|release) {
    message(DEBUG)
    DESTDIR = bin/debug
    DEFINES -= QT_NO_DEBUG_OUTPUT
    OBJECTS_DIR = tmp/debug
    MOC_DIR = tmp/debug
    RCC_DIR = tmp/debug
    UI_DIR = tmp/debug
}
CONFIG(release, debug|release) {
    message(RELEASE)
    DESTDIR = bin/release
    DEFINES += QT_NO_DEBUG_OUTPUT
    OBJECTS_DIR = tmp/release
    MOC_DIR = tmp/release
    RCC_DIR = tmp/release
    UI_DIR = tmp/release
}

CONFIG += c++11 warn_on link_prl qt thread

macx {
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.7
    QMAKE_LFLAGS += -stdlib=libc++
    QMAKE_CXXFLAGS += -stdlib=libc++
}

INCLUDEPATH += external/include \
    src
LIBS += -Lexternal/lib \
    -lbiosig

RESOURCES = src/src.qrc
win32:RC_FILE = src/src.rc
ICON = src/sigviewer.icns
TRANSLATIONS += src/translations/sigviewer_de.ts \
    src/translations/sigviewer_en.ts

include(src/base/base.pri)
include(src/signal_processing/signal_processing.pri)
include(src/file_handling_impl/file_handling_impl.pri)
include(src/file_handling/file_handling.pri)
include(src/gui/gui.pri)
include(src/gui_impl/gui_impl.pri)
include(src/editing_commands/editing_commands.pri)
include(src/commands/commands.pri)
include(src/tests/tests.pri)

HEADERS += src/application_context_impl.h \
    src/file_context.h \
    src/tab_context.h \
    src/command_executer.h

SOURCES += src/main/sigviewer.cpp \
    src/application_context_impl.cpp \
    src/file_context.cpp \
    src/tab_context.cpp
