TEMPLATE = app
DESTDIR = ../bin
TARGET = sigviewer
CONFIG += warn_on \
    link_prl \
    qt \
    thread
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
    -lbiosig \
    -lfftw3
win32:LIBS += -lws2_32
win32:LIBS += ../extern/fftw3.lib
macx:LIBS += -lz
RESOURCES = src.qrc
win32:RC_FILE = src.rc
ICON = sigviewer.icns
TRANSLATIONS += translations\sigviewer_de.ts \
    translations\sigviewer_en.ts \
    translations\sigviewer_es.ts \
    translations\sigviewer_fr.ts
include(base/base.pri)
include(signal_browser/signal_browser.pri)
include(block_visualisation/block_visualisation.pri)
include(signal_processing/signal_processing.pri)
include(file_handling_impl/file_handling_impl.pri)
include(file_handling/file_handling.pri)
include(gui/gui.pri)
include(gui_impl/gui_impl.pri)
include(editing_commands/editing_commands.pri)
HEADERS += basic_header_info_dialog.h \
    event_color_manager.h \
    event_table_dialog.h \
    label_widget.h \
    log_dialog.h \
    main_window.h \
    main_window_model_impl.h \
    settings_dialog.h \
    signal_browser_mouse_handling.h \
    command_stack.h \
    event_time_selection_dialog.h \
    abstract_browser_model.h \
    gui_action_manager.h \
    application_context.h \
    file_context.h \
    tab_context.h \
    command_executer.h \
    application_context_impl.h \
    application_context_impl_interface.h
SOURCES += basic_header_info_dialog.cpp \
    event_color_manager.cpp \
    event_table_dialog.cpp \
    label_widget.cpp \
    log_dialog.cpp \
    main_window.cpp \
    main_window_model_impl.cpp \
    settings_dialog.cpp \
    signal_browser_mouse_handling.cpp \
    sigviewer.cpp \
    command_stack.cpp \
    event_time_selection_dialog.cpp \
    gui_action_manager.cpp \
    application_context.cpp \
    file_context.cpp \
    tab_context.cpp \
    application_context_impl.cpp
