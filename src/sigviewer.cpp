/*

    $Id: sigviewer.cpp,v 1.5 2009-03-27 07:57:30 cle1109 Exp $
    Copyright (C) Thomas Brunner,  2005,2006
		          Clemens Brunner, 2009
    This file is part of the "SigViewer" repository
    at http://biosig.sf.net/

    SigViewer is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 3
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/



// sigviewer.cpp

#include "base/user_types.h"
#include "main_window.h"
#include "main_window_model.h"
#include "base/event_table_file_reader.h"

#include <stdlib.h>

#include <QApplication>
#include <QTranslator>
#include <QTextCodec>
#include <QLocale>
#include <QFile>
#include <QTextStream>

#include <iostream>

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
    qt_translator.load(QString("qt_") + QLocale::languageToString(QLocale::c().language()) +
                       QString(getenv("QTDIR")) + "/translations");
    application.installTranslator(&qt_translator);

    QTranslator sigviewer_translator(0);
    sigviewer_translator.load(QString("sigviewer_") + QLocale::languageToString(QLocale::c().language()),
                              application.applicationDirPath());
    application.installTranslator(&sigviewer_translator);

    MainWindowModel main_window_model;
    MainWindow main_window(main_window_model);
    main_window_model.setMainWindow(&main_window);
    main_window_model.getEventTableFileReader()
        ->load(":/eventcodes.txt");
    main_window_model.loadSettings();
    main_window.setUnifiedTitleAndToolBarOnMac(true);
    main_window.show();

    if (application.arguments().count() > 1)
        main_window_model.openFile (application.arguments().at(1));

    int result = application.exec();
    main_window_model.saveSettings();

    return result;
}
