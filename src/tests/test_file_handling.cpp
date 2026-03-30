// © SigViewer developers
//
// License: GPL-3.0

#include "application_context.h"
#include "file_handling/file_signal_writer_factory.h"
#include "file_handling/file_signal_reader_factory.h"
#include "gui/commands/open_file_gui_command.h"
#include "gui/gui_action_factory.h"
#include "gui/gui_action_factory_registrator.h"
#include "mock_file_signal_reader.h"

#include <QApplication>
#include <QTemporaryFile>
#include <QtTest>

using namespace sigviewer;

class TestFileHandling : public QObject
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

    void init()
    {
        OpenFileGuiCommand::openFile("blub.sinusdummy");
    }

    void cleanup()
    {
        GuiActionFactory::getInstance()->getQAction(tr("Close"))->trigger();
    }

    void exportImportEvents()
    {
        auto ctx = ApplicationContext::getInstance()->getCurrentFileContext();
        QVERIFY(!ctx.isNull());
        auto evtMgr = ctx->getEventManager();

        // Export all events
        QTemporaryFile f1("XXXXXX.evt");
        QVERIFY(f1.open());
        f1.close();
        QSharedPointer<FileSignalWriter> writer1(
            FileSignalWriterFactory::getInstance()->getHandler(f1.fileName()));
        QVERIFY(!writer1.isNull());
        QVERIFY(writer1->save(ctx).isEmpty());

        QSharedPointer<FileSignalReader> reader1(
            FileSignalReaderFactory::getInstance()->getHandler(f1.fileName()));
        QVERIFY(!reader1.isNull());
        QCOMPARE(reader1->getEvents().size(),
                 static_cast<int>(evtMgr->getNumberOfEvents()));

        // Export a single event type
        QTemporaryFile f2("XXXXXX.evt");
        QVERIFY(f2.open());
        f2.close();
        QSharedPointer<FileSignalWriter> writer2(
            FileSignalWriterFactory::getInstance()->getHandler(f2.fileName()));
        QVERIFY(!writer2.isNull());
        std::set<EventType> types;
        types.insert(evtMgr->getEvent(0)->getType());
        QVERIFY(writer2->save(ctx, types).isEmpty());

        QSharedPointer<FileSignalReader> reader2(
            FileSignalReaderFactory::getInstance()->getHandler(f2.fileName()));
        auto events2 = reader2->getEvents();
        QCOMPARE(events2.size(),
                 evtMgr->getEvents(*types.begin()).size());

        for (EventID eid : evtMgr->getEvents(*types.begin())) {
            bool found = false;
            for (auto& ev : events2)
                if (ev->equals(*evtMgr->getEvent(eid)))
                    found = true;
            QVERIFY(found);
        }
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
    sigviewer::registerMockFileSignalReader();
    TestFileHandling test;
    return QTest::qExec(&test, argc, argv);
}

#include "test_file_handling.moc"
