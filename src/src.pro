TEMPLATE = app
QT += qt3support
TARGET = sigviewer
DESTDIR = ..\bin
MOC_DIR = ..\tmp
OBJECTS_DIR = ..\tmp
RCC_DIR = ..\tmp
CONFIG += debug \
    warn_on \
    qt \
    static \
    thread
RESOURCES = src.qrc
win32:RC_FILE = src.rc
TRANSLATIONS += translations\sigviewer_de.ts \
    translations\sigviewer_en.ts \
    translations\sigviewer_es.ts \
    translations\sigviewer_fr.ts
HEADERS += base\basic_header.h \
    base\file_signal_reader_factory.h \
    base\file_signal_reader.h \
    base\math_utils.h \
    base\prototype_factory.h \
    base\signal_buffer.h \
    base\signal_channel.h \
    base\signal_data_block.h \
    base\signal_data_block_queue.h \
    base\signal_event.h \
    base\stream_utils.h \
    base\user_types.h \
    base\gdf\gdf_event.h \
    base\gdf\gdf_header.h \
    base\gdf\gdf_reader.h \
    base\gdf\gdf_signal_header.h \
    base\bkr\bkr_header.h \
    base\cnt\cnt_setup_header.h \
    smart_canvas\smart_canvas.h \
    smart_canvas\smart_canvas_rectangle.h \
    smart_canvas\smart_canvas_view.h \
    basic_header_info_dialog.h \
    channel_selection_dialog.h \
    channel_separator_canvas_item.h \
    go_to_dialog.h \
    gui_signal_buffer.h \
    log_dialog.h \
    main_window.h \
    main_window_model.h \
    navigation_canvas_item.h \
    signal_browser.h \
    signal_browser_model.h \
    signal_canvas_item.h \
    x_axis_widget.h \
    x_grid_canvas_item.h \
    y_axis_widget.h \
    event_canvas_item.h \
    copy_event_dialog.h \
    event_type_dialog.h \
    event_table_dialog.h \
    base\file_signal_writer.h \
    base\file_signal_writer_factory.h \
    base\gdf\gdf_writer.h \
    base\event_table_file_reader.h \
    event_color_manager.h \
    signal_browser_mouse_handling.h \
    label_widget.h \
    base\bkr\bkr_reader.h
SOURCES += sigviewer.cpp \
    base\basic_header.cpp \
    base\file_signal_reader.cpp \
    base\file_signal_reader_factory.cpp \
    base\math_utils.cpp \
    base\signal_buffer.cpp \
    base\signal_data_block.cpp \
    base\signal_data_block_queue.cpp \
    base\user_types.cpp \
    base\gdf\gdf_header.cpp \
    base\gdf\gdf_reader.cpp \
    smart_canvas\smart_canvas.cpp \
    smart_canvas\smart_canvas_rectangle.cpp \
    smart_canvas\smart_canvas_view.cpp \
    basic_header_info_dialog.cpp \
    channel_selection_dialog.cpp \
    channel_separator_canvas_item.cpp \
    go_to_dialog.cpp \
    gui_signal_buffer.cpp \
    log_dialog.cpp \
    main_window.cpp \
    main_window_model.cpp \
    navigation_canvas_item.cpp \
    signal_browser.cpp \
    signal_browser_model.cpp \
    signal_canvas_item.cpp \
    x_axis_widget.cpp \
    x_grid_canvas_item.cpp \
    y_axis_widget.cpp \
    event_canvas_item.cpp \
    base\gdf\gdf_event.cpp \
    copy_event_dialog.cpp \
    event_type_dialog.cpp \
    event_table_dialog.cpp \
    base\file_signal_writer.cpp \
    base\file_signal_writer_factory.cpp \
    base\gdf\gdf_writer.cpp \
    base\signal_channel.cpp \
    base\event_table_file_reader.cpp \
    event_color_manager.cpp \
    base\signal_event.cpp \
    signal_browser_mouse_handling.cpp \
    label_widget.cpp \
    base\bkr\bkr_reader.cpp
