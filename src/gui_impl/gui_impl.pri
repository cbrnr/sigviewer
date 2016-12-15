HEADERS += src/gui_impl/gui_helper_functions.h \
    src/gui_impl/processed_signal_channel_manager.h \
    src/gui_impl/select_shown_channels_dialog.h \
    src/gui_impl/main_window.h \
    src/gui_impl/main_window_model_impl.h \
    src/gui_impl/signal_browser_mouse_handling.h
SOURCES += src/gui_impl/processed_signal_channel_manager.cpp \
    src/gui_impl/select_shown_channels_dialog.cpp \
    src/gui_impl/gui_helper_functions.cpp \
    src/gui_impl/main_window.cpp \
    src/gui_impl/main_window_model_impl.cpp \
    src/gui_impl/signal_browser_mouse_handling.cpp
FORMS += src/gui_impl/info_widgets/power_spectrum_info_widget.ui
include(dialogs/dialogs.pri)
include(signal_browser/signal_browser.pri)
include(commands/commands.pri)
include(event_table/event_table.pri)
