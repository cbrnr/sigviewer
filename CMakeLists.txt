cmake_minimum_required(VERSION 3.21)

project(sigviewer
    VERSION 0.6.5
    LANGUAGES CXX C
)

# -- Pinned versions of external dependencies --------------------------------
# These are the ONLY accepted versions. Change them here (and rebuild the
# pre-built artifacts) when upgrading a dependency.
set(LIBXDF_VERSION    "0.99.10")
set(LIBBIOSIG_VERSION "3.9.4")

# -- Language standards ------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# -- Qt ----------------------------------------------------------------------
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Xml Svg LinguistTools Test)
qt_standard_project_setup()

# -- External pre-built dependencies -----------------------------------------
# Override with -DDEPS_DIR=<path> if the libraries were built elsewhere.
set(DEPS_DIR "${CMAKE_SOURCE_DIR}/external"
    CACHE PATH "Directory containing built libxdf and libbiosig")

# The versions.cmake file is written by external/build_deps.cmake and records
# the versions of the libraries that are actually present in DEPS_DIR.
set(_DEPS_VERSIONS_FILE "${DEPS_DIR}/versions.cmake")
if(NOT EXISTS "${_DEPS_VERSIONS_FILE}")
    message(FATAL_ERROR
        "Dependency version file not found:\n"
        "  ${_DEPS_VERSIONS_FILE}\n\n"
        "Run the following command to build the required libraries:\n"
        "  cmake -P external/build_deps.cmake\n"
        "Or manually place the libraries in\n"
        "${DEPS_DIR}\n"
        "and create external/versions.cmake with:\n"
        "  set(LIBXDF_VERSION_INSTALLED    \"${LIBXDF_VERSION}\")\n"
        "  set(LIBBIOSIG_VERSION_INSTALLED \"${LIBBIOSIG_VERSION}\")"
    )
endif()
include("${_DEPS_VERSIONS_FILE}")

foreach(_lib IN ITEMS LIBXDF LIBBIOSIG)
    set(_required "${${_lib}_VERSION}")
    set(_found    "${${_lib}_VERSION_INSTALLED}")
    if(NOT _found STREQUAL _required)
        message(FATAL_ERROR
            "${_lib} version mismatch.\n"
            "  Required : ${_required}\n"
            "  Installed: ${_found}\n\n"
            "Run cmake -P external/build_deps.cmake to install the correct version."
        )
    endif()
endforeach()
unset(_lib)
unset(_required)
unset(_found)

# Create IMPORTED targets so consumers get include dirs for free via
# target_link_libraries.
add_library(dep_libxdf STATIC IMPORTED)
set_target_properties(dep_libxdf PROPERTIES
    IMPORTED_LOCATION             "${DEPS_DIR}/lib/libxdf.a"
    INTERFACE_INCLUDE_DIRECTORIES "${DEPS_DIR}/include"
)

add_library(dep_libbiosig STATIC IMPORTED)
set_target_properties(dep_libbiosig PROPERTIES
    IMPORTED_LOCATION             "${DEPS_DIR}/lib/libbiosig.a"
    INTERFACE_INCLUDE_DIRECTORIES "${DEPS_DIR}/include"
)

# -- Sources -----------------------------------------------------------------
qt_add_library(sigviewer_objects OBJECT
    src/application_context.cpp
    src/application_context.h
    src/command_executer.h
    src/file_context.cpp
    src/file_context.h
    src/tab_context.cpp
    src/tab_context.h

    # base
    src/base/data_block.cpp
    src/base/data_block.h
    src/base/exception.cpp
    src/base/exception.h
    src/base/fixed_data_block.cpp
    src/base/fixed_data_block.h
    src/base/math_utils.cpp
    src/base/math_utils.h
    src/base/signal_channel.cpp
    src/base/signal_channel.h
    src/base/signal_event.cpp
    src/base/signal_event.h
    src/base/application_states.h
    src/base/file_states.h
    src/base/sigviewer_user_types.h
    src/base/tab_states.h

    # commands
    src/commands/convert_file_command.cpp
    src/commands/convert_file_command.h
    src/commands/open_file_command.cpp
    src/commands/open_file_command.h

    # editing_commands
    src/editing_commands/change_channel_undo_command.cpp
    src/editing_commands/change_channel_undo_command.h
    src/editing_commands/change_type_undo_command.cpp
    src/editing_commands/change_type_undo_command.h
    src/editing_commands/delete_event_undo_command.cpp
    src/editing_commands/delete_event_undo_command.h
    src/editing_commands/macro_undo_command.cpp
    src/editing_commands/macro_undo_command.h
    src/editing_commands/new_event_undo_command.cpp
    src/editing_commands/new_event_undo_command.h
    src/editing_commands/resize_event_undo_command.cpp
    src/editing_commands/resize_event_undo_command.h

    # file_handling
    src/file_handling/basic_header.cpp
    src/file_handling/basic_header.h
    src/file_handling/channel_manager.cpp
    src/file_handling/channel_manager.h
    src/file_handling/event_manager.h
    src/file_handling/file_handler_factory.h
    src/file_handling/file_signal_reader.cpp
    src/file_handling/file_signal_reader.h
    src/file_handling/file_signal_reader_factory.cpp
    src/file_handling/file_signal_reader_factory.h
    src/file_handling/file_signal_writer.h
    src/file_handling/file_signal_writer_factory.cpp
    src/file_handling/file_signal_writer_factory.h

    # file_handling (impl)
    src/file_handling/biosig_basic_header.cpp
    src/file_handling/biosig_basic_header.h
    src/file_handling/biosig_reader.cpp
    src/file_handling/biosig_reader.h
    src/file_handling/biosig_writer.cpp
    src/file_handling/biosig_writer.h
    src/file_handling/channel_manager_proxy.cpp
    src/file_handling/channel_manager_proxy.h
    src/file_handling/detrend_channel_manager.cpp
    src/file_handling/detrend_channel_manager.h
    src/file_handling/file_channel_manager.cpp
    src/file_handling/file_channel_manager.h
    src/file_handling/event_manager.cpp
    src/file_handling/event_manager.h
    src/file_handling/event_table_file_reader.cpp
    src/file_handling/event_table_file_reader.h
    src/file_handling/evt_writer.cpp
    src/file_handling/evt_writer.h
    src/file_handling/file_handler_factory_registrator.h
    src/file_handling/down_sampling_thread.cpp
    src/file_handling/down_sampling_thread.h
    src/file_handling/xdf_reader.cpp
    src/file_handling/xdf_reader.h

    # gui
    src/gui/background_processes.cpp
    src/gui/background_processes.h
    src/gui/color_manager.cpp
    src/gui/color_manager.h
    src/gui/event_view.h
    src/gui/gui_action_command.cpp
    src/gui/gui_action_command.h
    src/gui/gui_action_factory.cpp
    src/gui/gui_action_factory.h
    src/gui/gui_action_factory_registrator.h
    src/gui/gui_helper_functions.cpp
    src/gui/gui_helper_functions.h
    src/gui/main_window.cpp
    src/gui/main_window.h
    src/gui/main_window_model.cpp
    src/gui/main_window_model.h
    src/gui/processed_signal_channel_manager.cpp
    src/gui/processed_signal_channel_manager.h
    src/gui/progress_bar.h
    src/gui/select_shown_channels_dialog.cpp
    src/gui/select_shown_channels_dialog.h
    src/gui/signal_browser_mouse_handling.cpp
    src/gui/signal_browser_mouse_handling.h
    src/gui/signal_view_settings.cpp
    src/gui/signal_view_settings.h
    src/gui/signal_visualisation_model.cpp
    src/gui/signal_visualisation_model.h
    src/gui/signal_visualisation_modes.h
    src/gui/signal_visualisation_view.h
    src/gui/info_widgets/power_spectrum_info_widget.ui

    # gui/commands
    src/gui/commands/adapt_channel_view_gui_command.cpp
    src/gui/commands/adapt_channel_view_gui_command.h
    src/gui/commands/adapt_event_view_gui_command.cpp
    src/gui/commands/adapt_event_view_gui_command.h
    src/gui/commands/close_file_gui_command.cpp
    src/gui/commands/close_file_gui_command.h
    src/gui/commands/detrend_gui_command.cpp
    src/gui/commands/detrend_gui_command.h
    src/gui/commands/event_editing_gui_command.cpp
    src/gui/commands/event_editing_gui_command.h
    src/gui/commands/help_gui_command.cpp
    src/gui/commands/help_gui_command.h
    src/gui/commands/mouse_mode_gui_command.cpp
    src/gui/commands/mouse_mode_gui_command.h
    src/gui/commands/open_file_gui_command.cpp
    src/gui/commands/open_file_gui_command.h
    src/gui/commands/save_gui_command.cpp
    src/gui/commands/save_gui_command.h
    src/gui/commands/signal_processing_gui_command.cpp
    src/gui/commands/signal_processing_gui_command.h
    src/gui/commands/undo_redo_gui_command.cpp
    src/gui/commands/undo_redo_gui_command.h
    src/gui/commands/zoom_gui_command.cpp
    src/gui/commands/zoom_gui_command.h

    # gui/dialogs
    src/gui/dialogs/about_dialog.ui
    src/gui/dialogs/basic_header_info_dialog.cpp
    src/gui/dialogs/basic_header_info_dialog.h
    src/gui/dialogs/channel_dialog.ui
    src/gui/dialogs/channel_selection_dialog.cpp
    src/gui/dialogs/channel_selection_dialog.h
    src/gui/dialogs/event_time_selection_dialog.cpp
    src/gui/dialogs/event_time_selection_dialog.h
    src/gui/dialogs/event_time_selection_dialog.ui
    src/gui/dialogs/event_type_selection_dialog.ui
    src/gui/dialogs/event_types_selection_dialog.cpp
    src/gui/dialogs/event_types_selection_dialog.h
    src/gui/dialogs/resampling_dialog.cpp
    src/gui/dialogs/resampling_dialog.h
    src/gui/dialogs/resampling_dialog.ui
    src/gui/dialogs/scale_channel_dialog.cpp
    src/gui/dialogs/scale_channel_dialog.h
    src/gui/dialogs/scale_channel_dialog.ui

    # gui/event_table
    src/gui/event_table/event_table_view_model.cpp
    src/gui/event_table/event_table_view_model.h
    src/gui/event_table/event_table_widget.cpp
    src/gui/event_table/event_table_widget.h
    src/gui/event_table/event_table_widget.ui

    # gui/signal_browser
    src/gui/signal_browser/adapt_browser_view_widget.cpp
    src/gui/signal_browser/adapt_browser_view_widget.h
    src/gui/signal_browser/adapt_browser_view_widget.ui
    src/gui/signal_browser/event_context_menu.cpp
    src/gui/signal_browser/event_context_menu.h
    src/gui/signal_browser/event_creation_widget.cpp
    src/gui/signal_browser/event_creation_widget.h
    src/gui/signal_browser/event_creation_widget.ui
    src/gui/signal_browser/event_editing_widget.cpp
    src/gui/signal_browser/event_editing_widget.h
    src/gui/signal_browser/event_editing_widget.ui
    src/gui/signal_browser/event_graphics_item.cpp
    src/gui/signal_browser/event_graphics_item.h
    src/gui/signal_browser/label_widget.cpp
    src/gui/signal_browser/label_widget.h
    src/gui/signal_browser/signal_browser_graphics_view.h
    src/gui/signal_browser/signal_browser_model_4.cpp
    src/gui/signal_browser/signal_browser_model_4.h
    src/gui/signal_browser/signal_browser_view.cpp
    src/gui/signal_browser/signal_browser_view.h
    src/gui/signal_browser/signal_graphics_item.cpp
    src/gui/signal_browser/signal_graphics_item.h
    src/gui/signal_browser/signal_grid_graphics_item.cpp
    src/gui/signal_browser/signal_grid_graphics_item.h
    src/gui/signal_browser/x_axis_widget_4.cpp
    src/gui/signal_browser/x_axis_widget_4.h
    src/gui/signal_browser/y_axis_widget_4.cpp
    src/gui/signal_browser/y_axis_widget_4.h

    # signal_processing
    src/signal_processing/FFTReal.cpp
    src/signal_processing/FFTReal.h
)

qt_add_resources(SIGVIEWER_RESOURCES src/src.qrc)
target_sources(sigviewer_objects PRIVATE ${SIGVIEWER_RESOURCES})

# AUTOUIC needs to know every directory that contains a .ui file.
set_target_properties(sigviewer_objects PROPERTIES
    AUTOUIC_SEARCH_PATHS
        "${CMAKE_SOURCE_DIR}/src/gui/dialogs;${CMAKE_SOURCE_DIR}/src/gui/event_table;${CMAKE_SOURCE_DIR}/src/gui/signal_browser;${CMAKE_SOURCE_DIR}/src/gui/info_widgets"
)

# -- Compile definitions -----------------------------------------------------
target_compile_definitions(sigviewer_objects PUBLIC
    VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
    VERSION_MINOR=${PROJECT_VERSION_MINOR}
    VERSION_BUILD=${PROJECT_VERSION_PATCH}
    $<$<NOT:$<CONFIG:Debug>>:QT_NO_DEBUG_OUTPUT>
)

# -- Include paths and link libraries ----------------------------------------
# The autogen include dir (for generated ui_*.h headers) must be propagated
# explicitly so that test targets that consume sigviewer_objects can find them.
target_include_directories(sigviewer_objects PUBLIC
    src
    "${CMAKE_CURRENT_BINARY_DIR}/sigviewer_objects_autogen/include"
)

target_link_libraries(sigviewer_objects PUBLIC
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Xml
    Qt6::Svg
    dep_libxdf
    dep_libbiosig
    # libbiosig is a static library and cannot carry its own transitive
    # dependencies.  On macOS and Windows it uses libiconv; on Windows it also
    # uses Winsock (ntohl/htonl/getaddrinfo).
    $<$<OR:$<PLATFORM_ID:Darwin>,$<PLATFORM_ID:Windows>>:iconv>
    $<$<PLATFORM_ID:Windows>:ws2_32>
)

# -- Main application --------------------------------------------------------
qt_add_executable(sigviewer src/main.cpp)
target_link_libraries(sigviewer PRIVATE sigviewer_objects)

# -- Platform-specific sources -----------------------------------------------
if(APPLE)
    target_sources(sigviewer PRIVATE src/sigviewer.icns)
    set_source_files_properties(src/sigviewer.icns PROPERTIES
        MACOSX_PACKAGE_LOCATION "Resources")
    set_target_properties(sigviewer PROPERTIES
        MACOSX_BUNDLE                   TRUE
        MACOSX_BUNDLE_BUNDLE_NAME       "SigViewer"
        MACOSX_BUNDLE_BUNDLE_VERSION    "${PROJECT_VERSION}"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}"
        MACOSX_DEPLOYMENT_TARGET        "11"
        MACOSX_BUNDLE_ICON_FILE         "sigviewer.icns"
        OUTPUT_NAME                     "SigViewer"
    )
elseif(WIN32)
    target_sources(sigviewer PRIVATE src/src.rc)
    set_target_properties(sigviewer PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

# -- Translations ------------------------------------------------------------
qt_add_translations(sigviewer
    TS_FILES
        src/translations/sigviewer_de.ts
        src/translations/sigviewer_en.ts
        src/translations/sigviewer_ru.ts
)

# -- Tests -------------------------------------------------------------------
enable_testing()

function(add_sigviewer_test name source)
    qt_add_executable(${name} ${source})
    target_link_libraries(${name} PRIVATE sigviewer_objects Qt6::Test)
    add_test(NAME ${name} COMMAND ${name})
    set_tests_properties(${name} PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")
endfunction()

add_sigviewer_test(test_data_block           src/tests/test_data_block.cpp)
add_sigviewer_test(test_color_manager        src/tests/test_color_manager.cpp)
add_sigviewer_test(test_event_manager        src/tests/test_event_manager.cpp)
add_sigviewer_test(test_editing_commands     src/tests/test_editing_commands.cpp)
add_sigviewer_test(test_event_table_widget   src/tests/test_event_table_widget.cpp)
add_sigviewer_test(test_file_handling        src/tests/test_file_handling.cpp)
add_sigviewer_test(test_gui                  src/tests/test_gui.cpp)
