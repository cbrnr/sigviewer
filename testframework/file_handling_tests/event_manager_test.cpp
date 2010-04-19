#include "event_manager_test.h"

#include "dummy_file_signal_reader.h"
#include "file_handling_impl/event_manager_impl.h"

using namespace BioSig_;

//-----------------------------------------------------------------------------
void EventManagerTest::init ()
{
    QSharedPointer<DummyFileSignalReader> dummy_reader (new DummyFileSignalReader);
    dummy_reader->open ("asdf");
    event_manager_ = new BioSig_::EventManagerImpl (dummy_reader);
}

//-----------------------------------------------------------------------------
void EventManagerTest::testCreatingEvents ()
{
    ChannelID channel_id = 1;
    unsigned pos = 10;
    unsigned duration = 100;
    EventType type = 5;

    QSharedPointer<SignalEvent const>  created_event =
            event_manager_->createEvent (channel_id, pos, duration, type);

    QVERIFY2(created_event->getChannel() == channel_id, "check channel");
    QVERIFY2(created_event->getPosition() == pos, "check pos");
    QVERIFY2(created_event->getDuration() == duration, "check duration");
    QVERIFY2(created_event->getType() == type, "check type");

    QSharedPointer<SignalEvent const> stored_event (event_manager_->getEvent(created_event->getId()));

    QVERIFY2(stored_event->getId() == created_event->getId(), "check existance");
    QVERIFY2(stored_event->getChannel() == channel_id, "check channel");
    QVERIFY2(stored_event->getPosition() == pos, "check pos");
    QVERIFY2(stored_event->getDuration() == duration, "check duration");
    QVERIFY2(stored_event->getType() == type, "check type");
}


//-----------------------------------------------------------------------------
void EventManagerTest::cleanup ()
{
    delete event_manager_;
    event_manager_ = 0;
}
