HEADERS += \
    $$PWD/gui_helper_functions.h \
    $$PWD/main_window.h \
    $$PWD/main_window_model_impl.h \
    $$PWD/processed_signal_channel_manager.h \
    $$PWD/signal_browser_mouse_handling.h \
    $$PWD/select_shown_channels_dialog.h

SOURCES += \
    $$PWD/gui_helper_functions.cpp \
    $$PWD/main_window.cpp \
    $$PWD/main_window_model_impl.cpp \
    $$PWD/processed_signal_channel_manager.cpp \
    $$PWD/select_shown_channels_dialog.cpp \
    $$PWD/signal_browser_mouse_handling.cpp

FORMS += \
    $$PWD/info_widgets/power_spectrum_info_widget.ui

include($$PWD/commands/commands.pri)
include($$PWD/dialogs/dialogs.pri)
include($$PWD/event_table/event_table.pri)
include($$PWD/signal_browser/signal_browser.pri)
