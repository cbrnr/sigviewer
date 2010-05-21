HEADERS += gui_impl/adapt_channel_view_gui_command.h \
    gui_impl/adapt_event_view_gui_command.h \
    gui_impl/close_file_gui_command.h \
    gui_impl/event_editing_gui_command.h \
    gui_impl/gui_helper_functions.h \
    gui_impl/help_gui_command.h \
    gui_impl/mouse_mode_gui_command.h \
    gui_impl/open_file_gui_command.h \
    gui_impl/processed_signal_channel_manager.h \
    gui_impl/save_gui_command.h \
    gui_impl/select_shown_channels_dialog.h \
    gui_impl/signal_processing_gui_command.h \
    gui_impl/undo_redo_gui_command.h \
    gui_impl/zoom_gui_command.h
SOURCES += gui_impl/adapt_channel_view_gui_command.cpp \
    gui_impl/adapt_event_view_gui_command.cpp \
    gui_impl/close_file_gui_command.cpp \
    gui_impl/event_editing_gui_command.cpp \
    gui_impl/gui_helper_functions.cpp \
    gui_impl/help_gui_command.cpp \
    gui_impl/mouse_mode_gui_command.cpp \
    gui_impl/open_file_gui_command.cpp \
    gui_impl/processed_signal_channel_manager.cpp \
    gui_impl/save_gui_command.cpp \
    gui_impl/select_shown_channels_dialog.cpp \
    gui_impl/signal_processing_gui_command.cpp \
    gui_impl/undo_redo_gui_command.cpp \
    gui_impl/zoom_gui_command.cpp
HEADERS += gui_impl/dialogs/basic_header_info_dialog.h \
    gui_impl/dialogs/channel_selection_dialog.h \
    gui_impl/dialogs/event_time_selection_dialog.h \
    gui_impl/dialogs/event_types_selection_dialog.h \
    gui_impl/dialogs/scale_channel_dialog.h \
    gui_impl/dialogs/event_table_editing_dialog.h
SOURCES += gui_impl/dialogs/basic_header_info_dialog.cpp \
    gui_impl/dialogs/channel_selection_dialog.cpp \
    gui_impl/dialogs/event_time_selection_dialog.cpp \
    gui_impl/dialogs/event_types_selection_dialog.cpp \
    gui_impl/dialogs/scale_channel_dialog.cpp \
    gui_impl/dialogs/event_table_editing_dialog.cpp
FORMS += gui_impl/dialogs/channel_dialog.ui \
    gui_impl/dialogs/event_time_selection_dialog.ui \
    gui_impl/dialogs/event_type_selection_dialog.ui \
    gui_impl/dialogs/scale_channel_dialog.ui \
    gui_impl/dialogs/event_table_dialog.ui
