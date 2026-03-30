// © SigViewer developers
//
// License: GPL-3.0

#include "application_context.h"
#include "gui/gui_action_factory.h"
#include "gui/gui_action_factory_registrator.h"
#include "gui/commands/open_file_gui_command.h"
#include "file_context.h"
#include "base/file_states.h"

#include <QApplication>
#include <QAction>
#include <QtTest>

using namespace sigviewer;

class TestGui : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase()
    {
        ApplicationContext::init({});
    }

    void cleanupTestCase()
    {
        ApplicationContext::cleanup();
    }

    void actionStatesWithNoFileOpen()
    {
        QStringList enabledOnStartup = {
            tr("Animations"),
            tr("Open..."),
            tr("Set Animation Duration"),
            tr("About"),
            tr("Exit"),
        };
        for (QAction* action : GuiActionFactory::getInstance()->getQActions()) {
            bool shouldBeEnabled = enabledOnStartup.contains(action->text());
            QVERIFY2(action->isEnabled() == shouldBeEnabled,
                     qPrintable(QString("Unexpected enabled state for action: %1")
                                    .arg(action->text())));
        }
    }

    void openAndCloseFile()
    {
        OpenFileGuiCommand::openFile("blub.sinusdummy");
        QVERIFY(!ApplicationContext::getInstance()->getCurrentFileContext().isNull());

        // Delete an event and undo
        ApplicationContext::getInstance()
            ->getCurrentSignalVisualisationModel()
            ->selectEvent(1);
        GuiActionFactory::getInstance()->getQAction(tr("Delete"))->trigger();
        QVERIFY(ApplicationContext::getInstance()
                    ->getCurrentFileContext()
                    ->getEventManager()
                    ->getEvent(1)
                    .isNull());

        GuiActionFactory::getInstance()->getQAction(tr("Undo"))->trigger();
        QVERIFY(!ApplicationContext::getInstance()
                     ->getCurrentFileContext()
                     ->getEventManager()
                     ->getEvent(1)
                     .isNull());

        ApplicationContext::getInstance()->getCurrentFileContext()
            ->setState(FILE_STATE_UNCHANGED);

        // Hide a channel
        ApplicationContext::getInstance()
            ->getCurrentSignalVisualisationModel()
            ->selectChannel(1);
        GuiActionFactory::getInstance()->getQAction(tr("Hide Channel"))->trigger();
        QVERIFY(ApplicationContext::getInstance()
                    ->getCurrentSignalVisualisationModel()
                    ->getShownChannels()
                    .count(1) == 0);

        GuiActionFactory::getInstance()->getQAction(tr("Close"))->trigger();
        QVERIFY(ApplicationContext::getInstance()->getCurrentFileContext().isNull());
    }
};

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QApplication::setOrganizationName("SigViewer");
    QApplication::setOrganizationDomain("http://github.com/cbrnr/sigviewer/");
    QApplication::setApplicationName("SigViewer");
    GuiActionFactoryRegistrator::registerActions();
    GuiActionFactory::getInstance()->initAllCommands();
    TestGui test;
    return QTest::qExec(&test, argc, argv);
}

#include "tst_gui.moc"
