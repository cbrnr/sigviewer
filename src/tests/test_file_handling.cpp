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

namespace {

// Minimal FileSignalReader that just tags its header with a fixed string, so
// a test can tell which of several registered handlers actually resolved.
class TaggedMockHeader : public BasicHeader
{
public:
    explicit TaggedMockHeader(QString const& tag) : BasicHeader(tag)
    {
        setFileTypeString(tag);
    }

    size_t getNumberOfSamples() const override { return 0; }
};

class TaggedMockReader : public FileSignalReader
{
public:
    explicit TaggedMockReader(QString tag) : tag_(std::move(tag)) {}

    QPair<FileSignalReader*, QString> createInstance(QString const&) override
    {
        return {new TaggedMockReader(tag_), ""};
    }

    QSharedPointer<DataBlock const> getSignalData(ChannelID, size_t, size_t) const override
    {
        return QSharedPointer<DataBlock const>();
    }

    QList<QSharedPointer<SignalEvent const>> getEvents() const override { return {}; }

    QSharedPointer<BasicHeader> getBasicHeader() override
    {
        if (!header_)
            header_ = QSharedPointer<BasicHeader>(new TaggedMockHeader(tag_));
        return header_;
    }

    QSharedPointer<BasicHeader const> getBasicHeader() const override
    {
        return const_cast<TaggedMockReader*>(this)->getBasicHeader();
    }

private:
    QString tag_;
    QSharedPointer<BasicHeader> header_;
};

} // namespace

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

    void compoundExtensionDispatch()
    {
        // A double extension like "xdf.gz" must resolve to a handler
        // registered under that compound key in preference to one
        // registered under the plain last segment ("gz").
        QVERIFY(FileSignalReaderFactory::getInstance()->registerHandler(
            "gz", QSharedPointer<FileSignalReader>(new TaggedMockReader("gz"))));
        QVERIFY(FileSignalReaderFactory::getInstance()->registerHandler(
            "mock.gz", QSharedPointer<FileSignalReader>(new TaggedMockReader("mock.gz"))));

        QSharedPointer<FileSignalReader> resolved(
            FileSignalReaderFactory::getInstance()->getHandler("sample.mock.gz"));
        QVERIFY(!resolved.isNull());
        QCOMPARE(resolved->getBasicHeader()->getFileTypeString(), QString("mock.gz"));

        // A file whose ending doesn't match any compound key still falls
        // back to the plain single-segment handler.
        QSharedPointer<FileSignalReader> fallback(
            FileSignalReaderFactory::getInstance()->getHandler("sample.other.gz"));
        QVERIFY(!fallback.isNull());
        QCOMPARE(fallback->getBasicHeader()->getFileTypeString(), QString("gz"));
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
