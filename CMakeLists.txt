cmake_minimum_required(VERSION 3.12)
project(SigViewer
	LANGUAGES CXX
	VERSION 0.6.4
)

option(SIGVIEWER_BIOSIG "Use biosig to load files" OFF)
option(SIGVIEWER_LIBXDF "Use libxdf to load XDF files" ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

add_executable(${PROJECT_NAME} WIN32
	src/application_context_impl.cpp
	src/base/data_block.cpp
	src/base/exception.cpp
	src/base/fixed_data_block.cpp
	src/base/math_utils.cpp
	src/base/signal_channel.cpp
	src/base/signal_event.cpp
	src/commands/convert_file_command.cpp
	src/commands/open_file_command.cpp
	src/editing_commands/change_channel_undo_command.cpp
	src/editing_commands/change_type_undo_command.cpp
	src/editing_commands/delete_event_undo_command.cpp
	src/editing_commands/macro_undo_command.cpp
	src/editing_commands/new_event_undo_command.cpp
	src/editing_commands/resize_event_undo_command.cpp
	src/file_context.cpp
	src/file_handling/basic_header.cpp
	src/file_handling/channel_manager.cpp
	src/file_handling/event_manager.h
	src/file_handling/file_signal_reader.cpp
	src/file_handling/file_signal_reader_factory.cpp
	src/file_handling/file_signal_writer_factory.cpp
	src/file_handling_impl/channel_manager_impl.cpp
	src/file_handling_impl/down_sampling_thread.cpp
	src/file_handling_impl/event_manager_impl.cpp
	src/file_handling_impl/event_table_file_reader.cpp
	src/gui/background_processes.cpp
	src/gui/color_manager.cpp
	src/gui/gui_action_command.cpp
	src/gui/gui_action_factory.cpp
	src/gui/signal_view_settings.cpp
	src/gui/signal_visualisation_model.cpp
	src/gui_impl/commands/adapt_channel_view_gui_command.cpp
	src/gui_impl/commands/adapt_event_view_gui_command.cpp
	src/gui_impl/commands/close_file_gui_command.cpp
	src/gui_impl/commands/event_editing_gui_command.cpp
	src/gui_impl/commands/help_gui_command.cpp
	src/gui_impl/commands/mouse_mode_gui_command.cpp
	src/gui_impl/commands/open_file_gui_command.cpp
	src/gui_impl/commands/save_gui_command.cpp
	src/gui_impl/commands/signal_processing_gui_command.cpp
	src/gui_impl/commands/undo_redo_gui_command.cpp
	src/gui_impl/commands/zoom_gui_command.cpp
	src/gui_impl/dialogs/about_dialog.ui
	src/gui_impl/dialogs/basic_header_info_dialog.cpp
	src/gui_impl/dialogs/channel_dialog.ui
	src/gui_impl/dialogs/channel_selection_dialog.cpp
	src/gui_impl/dialogs/event_time_selection_dialog.cpp
	src/gui_impl/dialogs/event_time_selection_dialog.ui
	src/gui_impl/dialogs/event_type_selection_dialog.ui
	src/gui_impl/dialogs/event_types_selection_dialog.cpp
	src/gui_impl/dialogs/resampling_dialog.cpp
	src/gui_impl/dialogs/resampling_dialog.ui
	src/gui_impl/dialogs/scale_channel_dialog.cpp
	src/gui_impl/dialogs/scale_channel_dialog.ui
	src/gui_impl/event_table/event_table_view_model.cpp
	src/gui_impl/event_table/event_table_widget.cpp
	src/gui_impl/event_table/event_table_widget.ui
	src/gui_impl/gui_helper_functions.cpp
	src/gui_impl/info_widgets/power_spectrum_info_widget.ui
	src/gui_impl/main_window.cpp
	src/gui_impl/main_window_model_impl.cpp
	src/gui_impl/processed_signal_channel_manager.cpp
	src/gui_impl/select_shown_channels_dialog.cpp
	src/gui_impl/signal_browser/adapt_browser_view_widget.cpp
	src/gui_impl/signal_browser/adapt_browser_view_widget.ui
	src/gui_impl/signal_browser/event_context_menu.cpp
	src/gui_impl/signal_browser/event_creation_widget.cpp
	src/gui_impl/signal_browser/event_creation_widget.ui
	src/gui_impl/signal_browser/event_editing_widget.cpp
	src/gui_impl/signal_browser/event_editing_widget.ui
	src/gui_impl/signal_browser/event_graphics_item.cpp
	src/gui_impl/signal_browser/label_widget.cpp
	src/gui_impl/signal_browser/signal_browser_model_4.cpp
	src/gui_impl/signal_browser/signal_browser_view.cpp
	src/gui_impl/signal_browser/signal_browser_graphics_view.h
	src/gui_impl/signal_browser/signal_graphics_item.cpp
	src/gui_impl/signal_browser/signal_grid_graphics_item.cpp
	src/gui_impl/signal_browser/x_axis_widget_4.cpp
	src/gui_impl/signal_browser/y_axis_widget_4.cpp
	src/gui_impl/signal_browser_mouse_handling.cpp
	src/main.cpp
	src/signal_processing/FFTReal.cpp
	src/signal_processing/notch_filter8.cpp
	src/tab_context.cpp
	src/tests/color_manager_tests.cpp
	src/tests/data_block_tests.cpp
	src/tests/editing_commands_tests.cpp
	src/tests/event_manager_tests.cpp
	src/tests/event_table_widget_tests.cpp
	src/tests/file_handling_tests.cpp
	src/tests/gui_tests.cpp
	src/tests/test.cpp
	src/tests/tests_dialog.cpp
	src/tests/tests_dialog.ui
	src/file_handling_impl/biosig_basic_header.cpp
	# resources
	src/src.qrc
	)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_14)
target_compile_definitions(${PROJECT_NAME} PUBLIC 
    "VERSION_MAJOR=${PROJECT_VERSION_MAJOR}"
    "VERSION_MINOR=${PROJECT_VERSION_MINOR}"
    "VERSION_BUILD=${PROJECT_VERSION_PATCH}"
	)
set_target_properties(${PROJECT_NAME} PROPERTIES AUTOUIC_SEARCH_PATHS "src/gui_impl/dialogs")
target_include_directories(${PROJECT_NAME} PRIVATE ${AUTOGEN_BUILD_DIR})

find_package(Qt5 REQUIRED COMPONENTS Widgets Xml)
find_package(Boost REQUIRED)
if(SIGVIEWER_BIOSIG)
	target_sources(${PROJECT_NAME} PRIVATE
		src/file_handling_impl/gdf/gdf_file_signal_writer.cpp
		src/file_handling_impl/gdf/gdf_signal_cache.cpp
		src/file_handling_impl/gdf/gdf_load_data_thread.cpp
		src/file_handling_impl/gdf/gdf_basic_header.cpp
		src/file_handling_impl/gdf/gdf_data_block.cpp
		src/file_handling_impl/gdf/gdf_file_signal_reader.cpp
		src/file_handling_impl/biosig_writer.cpp
		src/file_handling_impl/biosig_reader.cpp
		src/file_handling_impl/evt_writer.cpp
	)
else()
	target_compile_definitions(${PROJECT_NAME} PRIVATE NOBIOSIG)
endif()

#if(SIGVIEWER_XDF)
	add_subdirectory(libxdf)
	target_sources(${PROJECT_NAME} PRIVATE
		src/file_handling_impl/xdf_reader.cpp
	)
	target_link_libraries(${PROJECT_NAME} PRIVATE xdf)
#endif()

target_include_directories(${PROJECT_NAME} PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/external/include
	src/
)
target_link_libraries(${PROJECT_NAME} PUBLIC
	Qt5::Widgets
	Qt5::Xml
	Boost::boost
)

#RESOURCES = $$PWD/src/src.qrc
#win32:RC_FILE = $$PWD/src/src.rc
#ICON = $$PWD/src/sigviewer.icns

install(TARGETS ${PROJECT_NAME})