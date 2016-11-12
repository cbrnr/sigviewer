HEADERS += \
    $$PWD/biosig_basic_header.h \
    $$PWD/biosig_reader.h \
    $$PWD/biosig_writer.h \
    $$PWD/channel_manager_impl.h \
    $$PWD/event_manager_impl.h \
    $$PWD/event_table_file_reader.h \
    $$PWD/evt_writer.h \
    $$PWD/file_handler_factory_registrator.h \
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

SOURCES += \
    $$PWD/biosig_basic_header.cpp \
    $$PWD/biosig_reader.cpp \
    $$PWD/biosig_writer.cpp \
    $$PWD/channel_manager_impl.cpp \
    $$PWD/event_manager_impl.cpp \
    $$PWD/event_table_file_reader.cpp \
    $$PWD/evt_writer.cpp \
	$$PWD/loadxdf.cpp \
    $$PWD/xdf_reader.cpp \
    $$PWD/../../external/pugixml/pugixml.cpp \
    $$PWD/../../external/smarc/filtering.c \
    $$PWD/../../external/smarc/multi_stage.c \
    $$PWD/../../external/smarc/polyfilt.c \
    $$PWD/../../external/smarc/remez_lp.c \
    $$PWD/../../external/smarc/smarc.c \
    $$PWD/../../external/smarc/stage_impl.c

INCLUDEPATH += $$_PRO_FILE_PWD_/../external/pugixml \
    $$_PRO_FILE_PWD_/../external/smarc

