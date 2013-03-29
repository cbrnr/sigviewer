#include "base/sigviewer_user_types.h"
#include "base/exception.h"
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
#include <QDir>

#include <iostream>
#include <string>

using namespace SigViewer_;


namespace SigViewer_
{

    //-------------------------------------------------------------------------
    /// readCommandlineParameters
    /// @return a map which may contain the following parameter values:
    ///         "input-file" -> an std::string containing the input file name
    ///         "test"       -> if test mode is activated
    ///         "convert-to-gdf" ->
    QMap<QString, QString> readCommandlineParameters(QStringList const& parameters);
}

//-----------------------------------------------------------------------------
// main
int main(int argc, char* argv[])
{
    qDebug() << "Starting SigViewer... (compiled with " << __GNUC__ << "."
             << __GNUC_MINOR__ << "." << __GNUC_PATCHLEVEL__ << ")";
    QApplication application(argc, argv);
    QApplication::setOrganizationName("BCI Lab");
    QApplication::setOrganizationDomain("http://bci.tugraz.at/");
    QApplication::setApplicationName("SigViewer");

    try
    {
        QMap<QString, QString> parameter_map = readCommandlineParameters(application.arguments());

        QString input_file_name;
        std::set<ApplicationMode> app_modes;
        if (parameter_map.count("help"))
            return 1;
        if (parameter_map.contains("input-file"))
            input_file_name = parameter_map["input-file"];
        if (parameter_map.contains("test"))
            app_modes.insert(APPLICATION_TEST_MODE);
        if (parameter_map.contains("convert-to-gdf"))
            app_modes.insert(APPLICATION_NON_GUI_MODE);
        if (parameter_map.contains("event-codes"))
        {
            QSettings settings;
            if (parameter_map["event-codes"].size())
                settings.setValue("eventcodes_file", parameter_map["event-codes"]);
            else
                settings.remove("eventcodes_file");
        }

        GuiActionFactory::getInstance()->initAllCommands();
        ApplicationContextImpl::init(app_modes);

        if (app_modes.count(APPLICATION_TEST_MODE))
            GuiActionFactory::getInstance()->getQAction("Running tests...")->trigger();
        else if (parameter_map.contains("convert-to-gdf"))
        {
            std::cout << "Converting to GDF is in testing phase. Meta-data will not be converted." << std::endl;
            ConvertFileCommand convert_command(ApplicationContextImpl::getInstance(), input_file_name, parameter_map["output-file"]);
            QString error = convert_command.execute();
            if (error.size())
                std::cout << error.toStdString() << std::endl;
            return 0;
        }
        else if (input_file_name.size())
            OpenFileGuiCommand::openFile(input_file_name);

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


namespace SigViewer_
{
    //-----------------------------------------------------------------------------
    QMap<QString, QString> readCommandlineParameters(QStringList const& parameters)
    {
        QMap<QString, QString> map;
        int start_index = 0;

        if (QDir::toNativeSeparators(QApplication::applicationFilePath()).contains(QDir::toNativeSeparators(parameters[0].mid(2, parameters[0].size() - 2))))
            start_index = 1;

        std::cout << QApplication::applicationFilePath().toStdString() << " - " << parameters[0].toStdString() << " - " << start_index << std::endl;
        for (int i = start_index; i < parameters.size(); i++)
        {
            if (parameters[i] == "--test" || parameters[i] == "-t")
                map.insert("test", "");
            else if (parameters[i] == "--output-file" || parameters[i] == "-o")
            {
                i++;
                if (i < parameters.size())
                    map.insert ("output-file", parameters[i]);
            }
            else if (parameters[i] == "--event-codes" || parameters[i] == "-e")
            {
                i++;
                if (i < parameters.size())
                    map.insert("event-codes", parameters[i]);
                else
                    throw Exception("event-codes parameter missing!");
            }
            else if (parameters[i] == "--convert-to-gdf" || parameters[i] == "-c")
                map.insert("convert-to-gdf", "");
            else
            {
                if (map.contains("input-file"))
                    throw Exception(map["input-file"].toStdString() + "Only 1 file can be opened!");
                map.insert ("input-file", parameters[i]);
            }
        }
        return map;
    }

}
