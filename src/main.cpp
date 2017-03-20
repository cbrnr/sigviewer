// Copyright (c) 2017 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


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

    #ifdef Q_OS_MAC
        QApplication::setAttribute(Qt::AA_DontShowIconsInMenus);
    #endif

    QCommandLineParser parser;
    parser.setApplicationDescription("SigViewer - a biosignal viewer.");
    parser.addPositionalArgument("file", QApplication::translate("main", "Input file (optional)."));
    parser.addHelpOption();
    parser.addVersionOption();
    parser.process(app);

    const QStringList args = parser.positionalArguments();

    GuiActionFactory::getInstance()->initAllCommands();
    std::set<ApplicationMode> app_modes;
    ApplicationContextImpl::init(app_modes);

    if (!args.isEmpty())
        OpenFileGuiCommand::openFile(args[0]);

    int result = app.exec();

    ApplicationContextImpl::cleanup();

    return result;
}
