TEMPLATE = app
QT += qt3support
TARGET = sigviewer
DESTDIR = ../bin
MOC_DIR = ../tmp
OBJECTS_DIR = ../tmp
RCC_DIR = ../tmp
INCLUDEPATH += ../extern
LIBS += -L../extern \
    -lbiosig
win32:LIBS += -lws2_32
CONFIG += warn_on \
    link_prl \
    qt \
    thread \
    x86 \
    ppc \
    release
RESOURCES = src.qrc
win32:RC_FILE = src.rc
ICON = sigviewer.icns
TRANSLATIONS += translations\sigviewer_de.ts \
    translations\sigviewer_en.ts \
    translations\sigviewer_es.ts \
    translations\sigviewer_fr.ts
    
include(base/base.pri)
include(smart_canvas/smart_canvas.pri)	
    
HEADERS += basic_header_info_dialog.h \
    channel_selection_dialog.h \
    channel_separator_canvas_item.h \
    copy_event_dialog.h \
    event_canvas_item.h \
    event_color_manager.h \
    event_table_dialog.h \
    event_type_dialog.h \
    go_to_dialog.h \
    gui_signal_buffer.h \
    label_widget.h \
    log_dialog.h \
    main_window.h \
    main_window_model.h \
    navigation_canvas_item.h \
    settings_dialog.h \
    signal_browser.h \
    signal_browser_model.h \
    signal_browser_mouse_handling.h \
    signal_canvas_item.h \
    x_axis_widget.h \
    x_grid_canvas_item.h \
    y_axis_widget.h
SOURCES += basic_header_info_dialog.cpp \
    channel_selection_dialog.cpp \
    channel_separator_canvas_item.cpp \
    copy_event_dialog.cpp \
    event_canvas_item.cpp \
    event_color_manager.cpp \
    event_table_dialog.cpp \
    event_type_dialog.cpp \
    go_to_dialog.cpp \
    gui_signal_buffer.cpp \
    label_widget.cpp \
    log_dialog.cpp \
    main_window.cpp \
    main_window_model.cpp \
    navigation_canvas_item.cpp \
    settings_dialog.cpp \
    signal_browser.cpp \
    signal_browser_model.cpp \
    signal_browser_mouse_handling.cpp \
    signal_canvas_item.cpp \
    sigviewer.cpp \
    x_axis_widget.cpp \
    x_grid_canvas_item.cpp \
    y_axis_widget.cpp
