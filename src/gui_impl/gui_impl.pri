HEADERS += gui_impl/gui_helper_functions.h \
    gui_impl/processed_signal_channel_manager.h \
    gui_impl/select_shown_channels_dialog.h \
    gui_impl/main_window.h \
    gui_impl/main_window_model_impl.h \
    gui_impl/signal_browser_mouse_handling.h
SOURCES += gui_impl/processed_signal_channel_manager.cpp \
    gui_impl/select_shown_channels_dialog.cpp \
    gui_impl/gui_helper_functions.cpp \
    gui_impl/main_window.cpp \
    gui_impl/main_window_model_impl.cpp \
    gui_impl/signal_browser_mouse_handling.cpp
FORMS += gui_impl/info_widgets/power_spectrum_info_widget.ui
include(dialogs/dialogs.pri)
include(signal_browser/signal_browser.pri)
include(commands/commands.pri)
include(event_table/event_table.pri)
