TEMPLATE = app
DESTDIR = ../bin
TARGET = sigviewer
CONFIG += warn_on \
    link_prl \
    qt \
    thread \
    x86 \
    ppc
debug { 
    OBJECTS_DIR = ../tmp/debug
    MOC_DIR = ../tmp/debug
    RCC_DIR = ../tmp/debug
    UI_DIR = ../tmp/debug
}
else:release { 
    OBJECTS_DIR = ../tmp/release
    MOC_DIR = ../tmp/release
    RCC_DIR = ../tmp/release
    UI_DIR = ../tmp/release
}

INCLUDEPATH += ../extern
LIBS += -L../extern \
    -lbiosig
win32:LIBS += -lws2_32
RESOURCES = src.qrc
win32:RC_FILE = src.rc
ICON = sigviewer.icns
TRANSLATIONS += translations\sigviewer_de.ts \
    translations\sigviewer_en.ts \
    translations\sigviewer_es.ts \
    translations\sigviewer_fr.ts
include(base/base.pri)
include(signal_browser/signal_browser.pri)
HEADERS += basic_header_info_dialog.h \
    channel_selection_dialog.h \
    copy_event_dialog.h \
    event_color_manager.h \
    event_table_dialog.h \
    event_type_dialog.h \
    go_to_dialog.h \
    gui_signal_buffer.h \
    label_widget.h \
    log_dialog.h \
    main_window.h \
    main_window_model.h \
    settings_dialog.h \
    signal_browser_mouse_handling.h \
    command_stack.h \
    next_event_view_undo_command.h
SOURCES += basic_header_info_dialog.cpp \
    channel_selection_dialog.cpp \
    copy_event_dialog.cpp \
    event_color_manager.cpp \
    event_table_dialog.cpp \
    event_type_dialog.cpp \
    go_to_dialog.cpp \
    gui_signal_buffer.cpp \
    label_widget.cpp \
    log_dialog.cpp \
    main_window.cpp \
    main_window_model.cpp \
    settings_dialog.cpp \
    signal_browser_mouse_handling.cpp \
    sigviewer.cpp \
    command_stack.cpp \
    next_event_view_undo_command.cpp
