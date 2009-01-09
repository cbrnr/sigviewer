/*

    $Id: sigviewer.cpp,v 1.3 2009-01-09 11:29:57 schloegl Exp $
    Copyright (C) Thomas Brunner  2005,2006
		  Clemens Brunner 2005,2008
    		  Alois Schloegl  2009
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
#include <QFile>
#include <QTextStream>

// 2009-01-09 added by AS
char FLAG_NOCACHE = 0; 		// used in base/biosig/biosig_reader.cpp to turn off caching of data. 
int SIGVIEWER_VERBOSE_LEVEL = 0; // might be useful for debugging 

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

    if (argc<2)
    	;
    else 
    {
	// 2009-01-09 added by AS
    	for (int k=1; k<argc && argv[k][0]=='-'; k++)
    	if (!strcmp(argv[k],"-v") || !strcmp(argv[k],"--version") ) {
		fprintf(stdout,"SigViewer v0.2.5\n");
		fprintf(stdout,"Copyright (C) 2005,2006,2007,2008,2009 by ");
		fprintf(stdout,"Thomas Brunner, Christoph Eibel, Clemens Brunner, Alois Schlögl, Oliver Terbu\n");
		fprintf(stdout,"This is part of BioSig http://biosig.sf.net - the free and\n");
		fprintf(stdout,"open source software library for biomedical signal processing.\n\n");
		fprintf(stdout,"SigViewer is free software; you can redistribute it and/or modify\n");
		fprintf(stdout,"it under the terms of the GNU General Public License as published by\n");
		fprintf(stdout,"the Free Software Foundation; either version 3 of the License, or\n");
		fprintf(stdout,"(at your option) any later version.\n");
		return(0);
	}	
    	else if (!strcmp(argv[k],"-h") || !strcmp(argv[k],"--help") ) {
		fprintf(stdout,"\nusage: sigviewer [OPTIONS] \n");
		fprintf(stdout,"  Supported OPTIONS are:\n");
		fprintf(stdout,"   -v, --version\n\tprints version information\n");
		fprintf(stdout,"   -h, --help   \n\tprints this information\n");
		fprintf(stdout,"   -n, --nocache\n\tdoes not load file into cache\n");
		fprintf(stdout,"\n\n");
		return(0);
	}	
    	else if (!strcmp(argv[k],"-n") || !strcmp(argv[k],"--nocache") ) {
		FLAG_NOCACHE = 1; 
    	}
    	else if (!strncmp(argv[k],"-VERBOSE",2))  	{
	    	SIGVIEWER_VERBOSE_LEVEL = argv[k][strlen(argv[k])-1]-48;
	}
    }	

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
