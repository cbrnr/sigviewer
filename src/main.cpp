#include "base/sigviewer_user_types.h"
#include "base/exception.h"
#include "gui_impl/main_window.h"
#include "gui_impl/main_window_model_impl.h"
#include "application_context_impl.h"
#include "file_handling_impl/event_table_file_reader.h"
#include "gui/gui_action_factory.h"
#include "gui_impl/commands/open_file_gui_command.h"
#include "commands/convert_file_command.h"

#include <QApplication>
#include <QSettings>
#include <QDir>
#include <QSysInfo>
#include <QCommandLineParser>

#include <iostream>


using namespace sigviewer;


int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QApplication::setOrganizationName("SigViewer");
    QApplication::setOrganizationDomain("http://github.com/cbrnr/sigviewer/");
    QApplication::setApplicationName("SigViewer");
    QApplication::setApplicationVersion(QString("%1.%2.%3").arg(VERSION_MAJOR).arg(VERSION_MINOR).arg(VERSION_BUILD));

    if (QSysInfo::productType() == "macos" || QSysInfo::productType() == "osx")
        QApplication::setAttribute(Qt::AA_DontShowIconsInMenus);

    QCommandLineParser parser;
    parser.setApplicationDescription("SigViewer - a biosignal viewer.");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.process(app);

//    try
//    {
//        QMap<QString, QString> parameter_map = readCommandlineParameters(application.arguments());
//        QString input_file_name;
        std::set<ApplicationMode> app_modes;
//        if (parameter_map.count("help"))
//            return 1;
//        if (parameter_map.contains("input-file"))
//            input_file_name = parameter_map["input-file"];
//        if (parameter_map.contains("test"))
//            app_modes.insert(APPLICATION_TEST_MODE);
//        if (parameter_map.contains("convert-to-gdf"))
//            app_modes.insert(APPLICATION_NON_GUI_MODE);
//        if (parameter_map.contains("event-codes"))
//        {
//            QSettings settings;
//            if (parameter_map["event-codes"].size())
//                settings.setValue("eventcodes_file", parameter_map["event-codes"]);
//            else
//                settings.remove("eventcodes_file");
//        }

    GuiActionFactory::getInstance()->initAllCommands();
    ApplicationContextImpl::init(app_modes);

//        if (app_modes.count(APPLICATION_TEST_MODE))
//            GuiActionFactory::getInstance()->getQAction("Running tests...")->trigger();
//        else if (parameter_map.contains("convert-to-gdf"))
//        {
//            std::cout << "Converting to GDF is in testing phase. Meta-data will not be converted." << std::endl;
//            ConvertFileCommand convert_command(ApplicationContextImpl::getInstance(), input_file_name, parameter_map["output-file"]);
//            QString error = convert_command.execute();
//            if (error.size())
//                std::cout << error.toStdString() << std::endl;
//            return 0;
//        }
//        else if (input_file_name.size())
//            OpenFileGuiCommand::openFile(input_file_name);

    int result = app.exec();

    ApplicationContextImpl::cleanup();
    return result;
//    }
//    catch (std::exception& e)
//    {
//        std::cerr << "Exception " << e.what() << std::endl;
//    }
    return 0;
}
