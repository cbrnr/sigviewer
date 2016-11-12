HEADERS += file_handling_impl/event_manager_impl.h \
    file_handling_impl/biosig_reader.h \
    file_handling_impl/biosig_writer.h \
    file_handling_impl/evt_writer.h \
    file_handling_impl/event_table_file_reader.h \
    file_handling_impl/channel_manager_impl.h \
    file_handling_impl/biosig_basic_header.h \
    file_handling_impl/file_handler_factory_registrator.h \ #\
    $$PWD/loadxdf.h \
    $$PWD/xdf_reader.h \
    $$PWD/../../external/pugixml/pugiconfig.hpp \
    $$PWD/../../external/pugixml/pugixml.hpp \
    $$PWD/../../external/smarc/filtering.h \
    $$PWD/../../external/smarc/multi_stage.h \
    $$PWD/../../external/smarc/polyfilt.h \
    $$PWD/../../external/smarc/remez_lp.h \
    $$PWD/../../external/smarc/smarc.h \
    $$PWD/../../external/smarc/stage_impl.h
    #file_handling_impl/gdf/gdf_file_signal_writer.h \
    #file_handling_impl/gdf/gdf_file_signal_reader.h \
    #file_handling_impl/gdf/gdf_basic_header.h \
    #file_handling_impl/gdf/gdf_data_block.h \
    #file_handling_impl/signal_cache.h # \
    #file_handling_impl/gdf/gdf_signal_cache.h \ #\
    #file_handling_impl/down_sampling_thread.h
    #file_handling_impl/gdf/gdf_load_data_thread.h
SOURCES += file_handling_impl/event_manager_impl.cpp \
    file_handling_impl/biosig_reader.cpp \
    file_handling_impl/biosig_writer.cpp \
    file_handling_impl/evt_writer.cpp \
    file_handling_impl/event_table_file_reader.cpp \
    file_handling_impl/channel_manager_impl.cpp \
    file_handling_impl/biosig_basic_header.cpp \
    $$PWD/loadxdf.cpp \
    $$PWD/xdf_reader.cpp \
    $$PWD/../../external/pugixml/pugixml.cpp \
    $$PWD/../../external/smarc/filtering.c \
    $$PWD/../../external/smarc/multi_stage.c \
    $$PWD/../../external/smarc/polyfilt.c \
    $$PWD/../../external/smarc/remez_lp.c \
    $$PWD/../../external/smarc/smarc.c \
    $$PWD/../../external/smarc/stage_impl.c
    # file_handling_impl/sinus_dummy_header.cpp #\
    #file_handling_impl/gdf/gdf_file_signal_writer.cpp \
    #file_handling_impl/gdf/gdf_file_signal_reader.cpp \
    #file_handling_impl/gdf/gdf_basic_header.cpp \
    #file_handling_impl/gdf/gdf_data_block.cpp \
    #file_handling_impl/gdf/gdf_signal_cache.cpp \ #\
    #file_handling_impl/down_sampling_thread.cpp
    #file_handling_impl/gdf/gdf_load_data_thread.cpp

INCLUDEPATH += $$_PRO_FILE_PWD_/../external/pugixml \
      $$_PRO_FILE_PWD_/../external/smarc
