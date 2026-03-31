// © SigViewer developers
//
// License: GPL-3.0


#include "gui/commands/open_file_gui_command.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QIcon>
#include <QTranslator>
#include <QLocale>
#include <QLibraryInfo>
#include <QPalette>


using namespace sigviewer;


int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QApplication::setOrganizationName("SigViewer");
    QApplication::setOrganizationDomain("http://github.com/cbrnr/sigviewer/");
    QApplication::setApplicationName("SigViewer");
    QApplication::setApplicationVersion(QString("%1.%2.%3").arg(VERSION_MAJOR).arg(VERSION_MINOR).arg(VERSION_BUILD));

    #ifdef Q_OS_MACOS
        QApplication::setAttribute(Qt::AA_DontShowIconsInMenus);
    #endif

    QTranslator translator;
    if (translator.load(QLocale::system(), QStringLiteral("sigviewer"), QStringLiteral("_"), QLibraryInfo::path(QLibraryInfo::TranslationsPath)))
    {
        QCoreApplication::installTranslator(&translator);
    }

    GuiActionFactoryRegistrator::registerActions();

    // Initialize icon theme for light/dark mode support
    QIcon::setThemeSearchPaths(QStringList{":/icons"} + QIcon::themeSearchPaths());
    QIcon::setFallbackThemeName("light");
    const bool darkMode = app.palette().color(QPalette::Window).lightness() < 128;
    QIcon::setThemeName(darkMode ? "dark" : "light");

    QCommandLineParser parser;
    parser.setApplicationDescription(QObject::tr("SigViewer - a biosignal viewer."));
    parser.addPositionalArgument("file", QApplication::translate("main", "Input file (optional)."));
    parser.addHelpOption();
    parser.addVersionOption();
    parser.process(app);

    const QStringList args = parser.positionalArguments();

    GuiActionFactory::getInstance()->initAllCommands();
    std::set<ApplicationMode> app_modes;
    ApplicationContext::init(app_modes);

    if (!args.isEmpty())
        OpenFileGuiCommand::openFile(args[0]);

    int result = app.exec();

    ApplicationContext::cleanup();

    return result;
}
