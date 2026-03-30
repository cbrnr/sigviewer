// © SigViewer developers
//
// License: GPL-3.0

#include "file_handling/event_manager.h"
#include "file_handling/file_signal_reader_factory.h"
#include "base/sigviewer_user_types.h"

#include <QApplication>
#include <QtTest>

using namespace sigviewer;

static QSharedPointer<EventManager> makeEventManager()
{
    FileSignalReader* reader =
        FileSignalReaderFactory::getInstance()->getHandler("blub.sinusdummy");
    QSharedPointer<EventManager> mgr(new EventManager(*reader));
    delete reader;
    return mgr;
}

class TestEventManager : public QObject
{
    Q_OBJECT

    QSharedPointer<EventManager> mgr_;

private slots:
    void init()  { mgr_ = makeEventManager(); }
    void cleanup() { mgr_.reset(); }

    void eventCount()
    {
        QCOMPARE(mgr_->getAllEvents().size(), 44);
    }

    void allEventsExist()
    {
        for (EventID id : mgr_->getAllEvents())
            QVERIFY(!mgr_->getEvent(id).isNull());
    }

    void createEvent()
    {
        auto event = mgr_->createEvent(1, 10, 100, 1, UNDEFINED_STREAM_ID);
        QVERIFY(!event.isNull());
        QCOMPARE(event->getChannel(), ChannelID(1));
        QCOMPARE(event->getDuration(), 100u);
        QCOMPARE(event->getDurationInSec(), 100.0 / mgr_->getSampleRate());
        QCOMPARE(event->getPosition(), 10u);
        QCOMPARE(event->getType(), EventType(1));
    }
};

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    TestEventManager test;
    return QTest::qExec(&test, argc, argv);
}

#include "tst_event_manager.moc"
