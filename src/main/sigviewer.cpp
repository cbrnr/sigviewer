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

#include "../base/sigviewer_user_types.h"
#include "gui_impl/main_window.h"
#include "gui_impl/main_window_model_impl.h"
#include "../application_context_impl.h"
#include "../file_handling_impl/event_table_file_reader.h"
#include "../gui/gui_action_factory.h"
#include "../gui_impl/commands/open_file_gui_command.h"

#include <stdlib.h>

#include <QApplication>
#include <QTranslator>
#include <QTextCodec>
#include <QLocale>
#include <QFile>
#include <QTextStream>
#include <QDebug>

#include <iostream>

using namespace BioSig_;

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
    try
    {
        qDebug () << "Starting SigViewer... (compiled with " << __GNUC__ << "." << __GNUC_MINOR__ << "." << __GNUC_PATCHLEVEL__ << ")";
        QApplication application(argc,argv);
        QTranslator qt_translator(0);
        qt_translator.load(QString("qt_") + QLocale::languageToString(QLocale::c().language()) +
                           QString(getenv("QTDIR")) + "/translations");
        application.installTranslator(&qt_translator);

        QTranslator sigviewer_translator(0);
        sigviewer_translator.load(QString("sigviewer_") + QLocale::languageToString(QLocale::c().language()),
                                  application.applicationDirPath());
        application.installTranslator(&sigviewer_translator);

        GuiActionFactory::getInstance()->initAllCommands ();
        ApplicationContextImpl::init();

        if (application.arguments().count() > 1)
        {
            QString file_argument;
            QStringList app_args = application.arguments();
            app_args.pop_front();
            foreach (QString argument, app_args)
                if (argument != "-test")
                    file_argument = argument;
            if (app_args.contains("-test"))
                GuiActionFactory::getInstance()->getQAction("Run Tests...")->trigger();
            else if (file_argument.size())
                OpenFileGuiCommand::openFile (file_argument);
        }

        int result = application.exec();

        ApplicationContextImpl::cleanup();
        return result;
    }
    catch (std::exception& e)
    {
        std::cerr << "exception caught: " << e.what() << std::endl;
    }
    return 0;
}
