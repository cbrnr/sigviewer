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
FORMS += gui_impl/info_widgets/power_spectrum_info_widget.ui
include(dialogs/dialogs.pri)
include(signal_browser/signal_browser.pri)
