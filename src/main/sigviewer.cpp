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

#include "base/sigviewer_user_types.h"
#include "gui_impl/main_window.h"
#include "gui_impl/main_window_model_impl.h"
#include "application_context_impl.h"
#include "file_handling_impl/event_table_file_reader.h"
#include "gui/gui_action_factory.h"
#include "gui_impl/commands/open_file_gui_command.h"
#include "commands/convert_file_command.h"

#include <stdlib.h>

#include <QApplication>
#include <QTranslator>
#include <QTextCodec>
#include <QLocale>
#include <QFile>
#include <QTextStream>
#include <QSettings>
#include <QDebug>

#include <boost/program_options.hpp>

#include <iostream>
#include <string>

using namespace SigViewer_;
using namespace boost;


namespace SigViewer_
{

    //-------------------------------------------------------------------------
    /// readCommandlineParameters
    /// @return a map which may contain the following parameter values:
    ///         "input-file" -> and std::string containing the input file name
    ///         "test"       -> if test mode is activated
    ///         "convert-to-gdf" ->
    program_options::variables_map readCommandlineParameters (int argc, char* argv[]);
}

//-----------------------------------------------------------------------------
// main
int main (int argc, char* argv[])
{
    qDebug () << "Starting SigViewer... (compiled with " << __GNUC__ << "."
              << __GNUC_MINOR__ << "." << __GNUC_PATCHLEVEL__ << ")";
    QApplication application (argc,argv);
    QApplication::setOrganizationName ("BCI Lab");
    QApplication::setOrganizationDomain("http://bci.tugraz.at");
    QApplication::setApplicationName ("SigViewer");

    try
    {
        program_options::variables_map parameter_map = readCommandlineParameters (argc, argv);

        QString input_file_name;
        std::set<ApplicationMode> app_modes;
        if (parameter_map.count ("help"))
            return 1;
        if (parameter_map.count ("input-file"))
            input_file_name = QString(parameter_map["input-file"].as<std::string>().c_str ());
        if (parameter_map.count ("test"))
            app_modes.insert (APPLICATION_TEST_MODE);
        if (parameter_map.count ("convert-to-gdf"))
            app_modes.insert (APPLICATION_NON_GUI_MODE);
        if (parameter_map.count ("event-codes"))
        {
            if (parameter_map["event-codes"].as<std::string>().size ())
            {
                QSettings settings;
                settings.setValue ("eventcodes_file", parameter_map["event-codes"].as<std::string>().c_str ());
            }
        }

        GuiActionFactory::getInstance()->initAllCommands ();
        ApplicationContextImpl::init (app_modes);

        if (app_modes.count (APPLICATION_TEST_MODE))
            GuiActionFactory::getInstance()->getQAction("Run Tests...")->trigger ();
        else if (parameter_map.count ("convert-to-gdf"))
        {
            std::cout << "Attention: Converting to GDF is in testing phase. Meta-data will not be converted." << std::endl;
            ConvertFileCommand convert_command (ApplicationContextImpl::getInstance(), input_file_name, parameter_map["output-file"].as<std::string>().c_str());
            QString error = convert_command.execute ();
            if (error.size ())
                std::cout << error.toStdString() << std::endl;
            return 0;
        }
        else if (input_file_name.size())
            OpenFileGuiCommand::openFile (input_file_name);

        int result = application.exec();

        ApplicationContextImpl::cleanup();
        return result;
    }
    catch (boost::exception&)
    {
        return 0;
    }
    catch (std::exception& e)
    {
        std::cerr << "exception caught: " << e.what() << std::endl;
    }
    return 0;
}


namespace SigViewer_
{
    //-----------------------------------------------------------------------------
    program_options::variables_map readCommandlineParameters (int argc, char* argv[])
    {
        program_options::options_description desc ("Allowed options");
        desc.add_options()
                ("test,t", "run test mode")
                ("help,h", "produce help message")
                ("input-file", program_options::value<std::string>(), "input file")
                ("convert-to-gdf,c", "converts the input file into gdf and saves it to the file given by the option '-o'")
                ("output-file,o", program_options::value<std::string>()->default_value("output.gdf"), "output file name for converted files (default is 'output.gdf')")
                ("event-codes,e", program_options::value<std::string>()->default_value(""), "event codes textfile")
        ;

        program_options::variables_map vm;
        try
        {
            program_options::positional_options_description pos_desc;
            pos_desc.add ("input-file", 1);

            program_options::store (program_options::command_line_parser (argc, argv).options(desc).positional(pos_desc).run(), vm);
            program_options::notify (vm);
        }
        catch (boost::exception&)
        {
            std::cerr << "Wrong commandline arguments!" << std::endl << std::endl;
            std::cout << desc;
            throw;
        }

        if (vm.count("help"))
            std::cout << desc;

        return vm;
    }

}
