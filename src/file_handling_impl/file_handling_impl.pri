HEADERS += src/file_handling_impl/event_manager_impl.h \
    src/file_handling_impl/biosig_reader.h \
    src/file_handling_impl/biosig_writer.h \
    src/file_handling_impl/evt_writer.h \
    src/file_handling_impl/event_table_file_reader.h \
    src/file_handling_impl/channel_manager_impl.h \
    src/file_handling_impl/biosig_basic_header.h \
    src/file_handling_impl/file_handler_factory_registrator.h #\
    #file_handling_impl/gdf/gdf_file_signal_writer.h \
    #file_handling_impl/gdf/gdf_file_signal_reader.h \
    #file_handling_impl/gdf/gdf_basic_header.h \
    #file_handling_impl/gdf/gdf_data_block.h \
    #file_handling_impl/signal_cache.h # \
    #file_handling_impl/gdf/gdf_signal_cache.h \ #\
    #file_handling_impl/down_sampling_thread.h
    #file_handling_impl/gdf/gdf_load_data_thread.h
SOURCES += src/file_handling_impl/event_manager_impl.cpp \
    src/file_handling_impl/biosig_reader.cpp \
    src/file_handling_impl/biosig_writer.cpp \
    src/file_handling_impl/evt_writer.cpp \
    src/file_handling_impl/event_table_file_reader.cpp \
    src/file_handling_impl/channel_manager_impl.cpp \
    src/file_handling_impl/biosig_basic_header.cpp
    # file_handling_impl/sinus_dummy_header.cpp #\
    #file_handling_impl/gdf/gdf_file_signal_writer.cpp \
    #file_handling_impl/gdf/gdf_file_signal_reader.cpp \
    #file_handling_impl/gdf/gdf_basic_header.cpp \
    #file_handling_impl/gdf/gdf_data_block.cpp \
    #file_handling_impl/gdf/gdf_signal_cache.cpp \ #\
    #file_handling_impl/down_sampling_thread.cpp
    #file_handling_impl/gdf/gdf_load_data_thread.cpp
