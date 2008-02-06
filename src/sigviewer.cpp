// sigviewer.cpp

#include "base/user_types.h"
#include "main_window.h"
#include "main_window_model.h"
#include "base/event_table_file_reader.h"

#include <stdlib.h>

#include <QApplication>
#include <QTranslator>
#include <QTextCodec>
#include <QFile>
#include <QTextStream>

using BioSig_::MainWindow;
using BioSig_::MainWindowModel;

void myMessageOutput( QtMsgType type, const char* msg )
{
    static bool first_call = true;
    static bool log_file_open = false;
    static QFile log_file( "sigviewer.log" );
    static QTextStream log_file_text_stream;
    if ( first_call )
    {
        first_call = false;
        log_file_open = log_file.open( QFile::WriteOnly );
        if ( log_file_open )
        {
            log_file_text_stream.setDevice( &log_file );
        }
    }

    if ( log_file_open )
    {
        switch ( type ) {
        case QtDebugMsg:
            log_file_text_stream << "Debug: " << msg << "\n";
            break;
        case QtWarningMsg:
            log_file_text_stream << "Warning: " << msg << "\n";
            break;
        case QtCriticalMsg:
            log_file_text_stream << "Critical: " << msg << "\n";
            break;
        case QtFatalMsg:
            log_file_text_stream << "Fatal: " << msg << "\n";
            abort();
        }
        log_file_text_stream.flush();
    }
}

void removeLogFile()
{
    QFile log_file( "sigviewer.log" );
    if ( log_file.exists() )
    {
        log_file.remove();
    }
}

// main
int main(int32 argc, char* argv[])
{
    QApplication application(argc,argv);
    removeLogFile();
    qInstallMsgHandler( myMessageOutput );

    QTranslator qt_translator(0);
    qt_translator.load(QString("qt_") + QTextCodec::locale(),
                       QString(getenv("QTDIR")) + "/translations");
    application.installTranslator(&qt_translator);

    QTranslator sigviewer_translator(0);
    sigviewer_translator.load(QString("sigviewer_") + QTextCodec::locale(),
                              application.applicationDirPath());
    application.installTranslator(&sigviewer_translator);

    MainWindowModel main_window_model;
    MainWindow main_window(main_window_model);
    main_window_model.setMainWindow(&main_window);
    main_window_model.getEventTableFileReader()
        .load((application.applicationDirPath() + "/eventcodes.txt").ascii());
    main_window_model.loadSettings();
    main_window.show();
    int result = application.exec();
    main_window_model.saveSettings();

    return result;
}
