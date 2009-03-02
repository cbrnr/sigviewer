TEMPLATE = app
QT += qt3support
TARGET = sigviewer
DESTDIR = ../bin
MOC_DIR = ../tmp
OBJECTS_DIR = ../tmp
RCC_DIR = ../tmp
INCLUDEPATH += ../extern
LIBS += -L../extern -lbiosig 
win32:LIBS += -lws2_32
CONFIG += warn_on \
    link_prl \
    qt \
    thread \
    release \
    x86 ppc 
RESOURCES = src.qrc
win32:RC_FILE = src.rc
ICON = sigviewer.icns
TRANSLATIONS += translations\sigviewer_de.ts \
    translations\sigviewer_en.ts \
    translations\sigviewer_es.ts \
    translations\sigviewer_fr.ts
HEADERS += *h \
    base\*h \
    base\evt\*h \
    base\biosig\*h \
    smart_canvas\*h
SOURCES += *.cpp \
    base\*.cpp \
    base\evt\*.cpp \
    base\biosig\*.cpp \
    smart_canvas\*.cpp
FORMS = 
