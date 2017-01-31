TEMPLATE = app
DESTDIR = bin
TARGET = sigviewer

VERSION_MAJOR = 0
VERSION_MINOR = 6
VERSION_BUILD = 0

DEFINES += \
    "VERSION_MAJOR=$$VERSION_MAJOR" \
    "VERSION_MINOR=$$VERSION_MINOR" \
    "VERSION_BUILD=$$VERSION_BUILD"

QT += core gui widgets xml

BUILD_DIR = $$PWD/tmp
TARGET_DIR = $$PWD/bin

CONFIG(debug, debug|release) {
    DEFINES -= QT_NO_DEBUG_OUTPUT
    DESTDIR = $$TARGET_DIR/debug
    OBJECTS_DIR = $$BUILD_DIR/debug
    MOC_DIR = $$BUILD_DIR/debug
    RCC_DIR = $$BUILD_DIR/debug
    UI_DIR = $$BUILD_DIR/debug
}
CONFIG(release, debug|release) {
    DEFINES += QT_NO_DEBUG_OUTPUT
    DESTDIR = $$TARGET_DIR/release
    OBJECTS_DIR = $$BUILD_DIR/release
    MOC_DIR = $$BUILD_DIR/release
    RCC_DIR = $$BUILD_DIR/release
    UI_DIR = $$BUILD_DIR/release
}

CONFIG += warn_on link_prl qt thread

macx {
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.9
    LIBS += -liconv
}

INCLUDEPATH += \
    $$PWD/external/include \
    $$PWD/src

LIBS += \
    -L$$PWD/external/lib \
    -lbiosig

RESOURCES = $$PWD/src/src.qrc
win32:RC_FILE = $$PWD/src/src.rc
ICON = $$PWD/src/sigviewer.icns

TRANSLATIONS += \
    $$PWD/src/translations/sigviewer_de.ts \
    $$PWD/src/translations/sigviewer_en.ts

include($$PWD/src/base/base.pri)
include($$PWD/src/signal_processing/signal_processing.pri)
include($$PWD/src/file_handling_impl/file_handling_impl.pri)
include($$PWD/src/file_handling/file_handling.pri)
include($$PWD/src/gui/gui.pri)
include($$PWD/src/gui_impl/gui_impl.pri)
include($$PWD/src/editing_commands/editing_commands.pri)
include($$PWD/src/commands/commands.pri)
include($$PWD/src/tests/tests.pri)

HEADERS += \
    $$PWD/src/application_context_impl.h \
    $$PWD/src/command_executer.h \
    $$PWD/src/file_context.h \
    $$PWD/src/tab_context.h

SOURCES += \
    $$PWD/src/main.cpp \
    $$PWD/src/application_context_impl.cpp \
    $$PWD/src/file_context.cpp \
    $$PWD/src/tab_context.cpp
